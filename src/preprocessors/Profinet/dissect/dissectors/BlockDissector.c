/**
 * @file
 * @brief BlockDissector implementation.
 *
 * This Dissector dissects pn blocks
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/dissectors/BlockDissector.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"
#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"

#include "dissect/DissectionUtils.h"

#include "dbg.h"



#define FRAME_ID_DCP_HELLO      0xfefc
#define FRAME_ID_DCP_GETORSET   0xfefd
#define FRAME_ID_DCP_IDENT_REQ  0xfefe
#define FRAME_ID_DCP_IDENT_RES  0xfeff


static int hf_pn_dcp_suboption_device = -1;
static int hf_pn_dcp_suboption_device_typeofstation = -1;
static int hf_pn_dcp_suboption_device_nameofstation = -1;
static int hf_pn_dcp_suboption_vendor_id = -1;
static int hf_pn_dcp_suboption_device_id = -1;
static int hf_pn_dcp_suboption_device_role = -1;
static int hf_pn_dcp_suboption_device_aliasname = -1;
static int hf_pn_dcp_suboption_device_instance_high = -1;
static int hf_pn_dcp_suboption_device_instance_low = -1;
static int hf_pn_dcp_suboption_device_oem_ven_id = -1;
static int hf_pn_dcp_suboption_device_oem_dev_id = -1;

static int hf_pn_dcp_suboption_dhcp = -1;
static int hf_pn_dcp_suboption_dhcp_device_id = -1;

static int hf_pn_dcp_suboption_control = -1;
static int hf_pn_dcp_suboption_control_response = -1;

static int hf_pn_dcp_suboption_deviceinitiative = -1;
static int hf_pn_dcp_deviceinitiative_value = -1;

static int hf_pn_dcp_suboption_all = -1;

static int hf_pn_dcp_suboption_manuf = -1;


static int ett_pn_dcp = -1;
static int ett_pn_dcp_block = -1;

//static expert_field ei_pn_dcp_block_error_unknown = EI_INIT;
//static expert_field ei_pn_dcp_ip_conflict = EI_INIT;


#define PNDCP_SERVICE_ID_GET        0x03
#define PNDCP_SERVICE_ID_SET        0x04
#define PNDCP_SERVICE_ID_IDENTIFY   0x05
#define PNDCP_SERVICE_ID_HELLO      0x06

static const value_string pn_dcp_service_id[] = {
    { 0x00,                     "reserved" },
    { 0x01,                     "Manufacturer specific" },
    { 0x02,                     "Manufacturer specific" },
    { PNDCP_SERVICE_ID_GET,     "Get" },
    { PNDCP_SERVICE_ID_SET,     "Set" },
    { PNDCP_SERVICE_ID_IDENTIFY,"Identify" },
    { PNDCP_SERVICE_ID_HELLO,   "Hello" },
    /* 0x07 - 0xff reserved */
    { 0, NULL }
};

#define PNDCP_SERVICE_TYPE_REQUEST              0
#define PNDCP_SERVICE_TYPE_RESPONSE_SUCCESS     1
#define PNDCP_SERVICE_TYPE_RESPONSE_UNSUPPORTED 5

static const value_string pn_dcp_service_type[] = {
    { PNDCP_SERVICE_TYPE_REQUEST,               "Request" },
    { PNDCP_SERVICE_TYPE_RESPONSE_SUCCESS,      "Response Success" },
    { PNDCP_SERVICE_TYPE_RESPONSE_UNSUPPORTED,  "Response - Request not supported" },
    /* all others reserved */
    { 0, NULL }
};

static const value_string pn_dcp_block_error[] = {
    { 0x00, "Ok" },
    { 0x01, "Option unsupp." },
    { 0x02, "Suboption unsupp. or no DataSet avail." },
    { 0x03, "Suboption not set" },
    { 0x04, "Resource Error" },
    { 0x05, "SET not possible by local reasons" },
    { 0x06, "In operation, SET not possible" },
    /* all others reserved */
    { 0, NULL }
};

static const value_string pn_dcp_block_info[] = {
    { 0x0000, "Reserved" },
    /*0x0001 - 0xffff reserved */
    { 0, NULL }
};

static const value_string pn_dcp_block_qualifier[] = {
    { 0x0000, "Use the value temporary" },
    { 0x0001, "Save the value permanent" },
    /*0x0002 - 0xffff reserved */
    { 0, NULL }
};

static const value_string pn_dcp_BlockQualifier[] = {
    { 0x0002, "Reset application data" },
    { 0x0003, "Reset application data" },
    { 0x0004, "Reset communication parameter" },
    { 0x0005, "Reset communication parameter" },
    { 0x0006, "Reset engineering parameter" },
    { 0x0007, "Reset engineering parameter" },
    { 0x0008, "Resets all stored data" },
    { 0x0009, "Resets all stored data" },
    { 0x000A, "Reset engineering parameter" },
    { 0x000B, "Reset engineering parameter" },
    { 0x000C, "Reserved" },
    { 0x000D, "Reserved" },
    { 0x0009, "Reserved" },
    { 0x0010, "Resets all stored data in the IOD or IOC to its factory values" },
    { 0x0011, "Resets all stored data in the IOD or IOC to its factory values" },
    { 0x0012, "Reset and restore data" },
    { 0x0013, "Reset and restore data" },
    { 0x0014, "Reserved" },
    { 0x0015, "Reserved" },
    { 0x0016, "Reserved" },
    { 0, NULL }
};

#define PNDCP_OPTION_IP                 0x01
#define PNDCP_OPTION_DEVICE             0x02
#define PNDCP_OPTION_DHCP               0x03
#define PNDCP_OPTION_RESERVED           0x04
#define PNDCP_OPTION_CONTROL            0x05
#define PNDCP_OPTION_DEVICEINITIATIVE   0x06
#define PNDCP_OPTION_MANUF_X80          0x80
#define PNDCP_OPTION_MANUF_X81          0x81
#define PNDCP_OPTION_MANUF_X82          0x82
#define PNDCP_OPTION_MANUF_X83          0x83
#define PNDCP_OPTION_MANUF_X84          0x84
#define PNDCP_OPTION_MANUF_X85          0x85
#define PNDCP_OPTION_MANUF_X86          0x86
#define PNDCP_OPTION_ALLSELECTOR        0xff

static const value_string pn_dcp_option[] = {
    { 0x00, "reserved" },
    { PNDCP_OPTION_IP,                  "IP" },
    { PNDCP_OPTION_DEVICE,              "Device properties" },
    { PNDCP_OPTION_DHCP,                "DHCP" },
    { PNDCP_OPTION_RESERVED,            "Reserved" },
    { PNDCP_OPTION_CONTROL,             "Control" },
    { PNDCP_OPTION_DEVICEINITIATIVE,    "Device Initiative" },
    /*0x07 - 0x7f reserved */
    /*0x80 - 0xfe manufacturer specific */
    { PNDCP_OPTION_MANUF_X80,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X81,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X82,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X83,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X84,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X85,           "Manufacturer specific" },
    { PNDCP_OPTION_MANUF_X86,           "Manufacturer specific" },
    { PNDCP_OPTION_ALLSELECTOR,         "All Selector" },
    { 0, NULL }
};

#define PNDCP_SUBOPTION_IP_MAC  0x01
#define PNDCP_SUBOPTION_IP_IP   0x02

static const value_string pn_dcp_suboption_ip[] = {
    { 0x00, "Reserved" },
    { PNDCP_SUBOPTION_IP_MAC,   "MAC address" },
    { PNDCP_SUBOPTION_IP_IP,    "IP parameter" },
    /*0x03 - 0xff reserved */
    { 0, NULL }
};

static const value_string pn_dcp_suboption_ip_block_info[] = {
    { 0x0000, "IP not set" },
    { 0x0001, "IP set" },
    { 0x0002, "IP set by DHCP" },
    { 0x0080, "IP not set (address conflict detected)" },
    { 0x0081, "IP set (address conflict detected)" },
    { 0x0082, "IP set by DHCP (address conflict detected)" },
    /*0x0003 - 0xffff reserved */
    { 0, NULL }
};

#define PNDCP_SUBOPTION_DEVICE_MANUF            0x01
#define PNDCP_SUBOPTION_DEVICE_NAMEOFSTATION    0x02
#define PNDCP_SUBOPTION_DEVICE_DEV_ID           0x03
#define PNDCP_SUBOPTION_DEVICE_DEV_ROLE         0x04
#define PNDCP_SUBOPTION_DEVICE_DEV_OPTIONS      0x05
#define PNDCP_SUBOPTION_DEVICE_ALIAS_NAME       0x06
#define PNDCP_SUBOPTION_DEVICE_DEV_INSTANCE     0x07
#define PNDCP_SUBOPTION_DEVICE_OEM_DEV_ID       0x08

static const value_string pn_dcp_suboption_device[] = {
    { 0x00, "Reserved" },
    { PNDCP_SUBOPTION_DEVICE_MANUF,         "Manufacturer specific (Type of Station)" },
    { PNDCP_SUBOPTION_DEVICE_NAMEOFSTATION, "Name of Station" },
    { PNDCP_SUBOPTION_DEVICE_DEV_ID,        "Device ID" },
    { PNDCP_SUBOPTION_DEVICE_DEV_ROLE,      "Device Role" },
    { PNDCP_SUBOPTION_DEVICE_DEV_OPTIONS,   "Device Options" },
    { PNDCP_SUBOPTION_DEVICE_ALIAS_NAME,    "Alias Name" },
    { PNDCP_SUBOPTION_DEVICE_DEV_INSTANCE,  "Device Instance" },
    { PNDCP_SUBOPTION_DEVICE_OEM_DEV_ID,    "OEM Device ID"},
    /*0x09 - 0xff reserved */
    { 0, NULL }
};

#define PNDCP_SUBOPTION_DHCP_CLIENT_ID  61

static const value_string pn_dcp_suboption_dhcp[] = {
    {  12, "Host name" },
    {  43, "Vendor specific" },
    {  54, "Server identifier" },
    {  55, "Parameter request list" },
    {  60, "Class identifier" },
    {  PNDCP_SUBOPTION_DHCP_CLIENT_ID, "DHCP client identifier" },
    {  81, "FQDN, Fully Qualified Domain Name" },
    {  97, "UUID/GUID-based Client" },
    { 255, "Control DHCP for address resolution" },
    /*all others reserved */
    { 0, NULL }
};

#define PNDCP_SUBOPTION_CONTROL_START_TRANS 0x01
#define PNDCP_SUBOPTION_CONTROL_END_TRANS   0x02
#define PNDCP_SUBOPTION_CONTROL_SIGNAL      0x03
#define PNDCP_SUBOPTION_CONTROL_RESPONSE    0x04
#define PNDCP_SUBOPTION_CONTROL_FACT_RESET  0x05
#define PNDCP_SUBOPTION_CONTROL_RESET_TO_FACT  0x06

static const value_string pn_dcp_suboption_control[] = {
    { 0x00, "Reserved" },
    { PNDCP_SUBOPTION_CONTROL_START_TRANS, "Start Transaction" },
    { PNDCP_SUBOPTION_CONTROL_END_TRANS,   "End Transaction" },
    { PNDCP_SUBOPTION_CONTROL_SIGNAL,      "Signal" },
    { PNDCP_SUBOPTION_CONTROL_RESPONSE,    "Response" },
    { PNDCP_SUBOPTION_CONTROL_FACT_RESET,  "Reset Factory Settings" },
    { PNDCP_SUBOPTION_CONTROL_RESET_TO_FACT,"Reset to Factory" },
    /*0x07 - 0xff reserved */
    { 0, NULL }
};

#define PNDCP_SUBOPTION_DEVICEINITIATIVE 0x01

static const value_string pn_dcp_suboption_deviceinitiative[] = {
    { 0x00, "Reserved" },
    { PNDCP_SUBOPTION_DEVICEINITIATIVE, "Device Initiative" },
    /*0x00 - 0xff reserved */
    { 0, NULL }
};

static const value_string pn_dcp_deviceinitiative_value[] = {
    { 0x00, "Device does not issue a DCP-Hello-ReqPDU after power on" },
    { 0x01, "Device does issue a DCP-Hello-ReqPDU after power on" },
    /*0x02 - 0xff reserved */
    { 0, NULL }
};

static const value_string pn_dcp_suboption_all[] = {
    { 0xff, "ALL Selector" },
    /* all other reserved */
    { 0, NULL }
};

static const value_string pn_dcp_suboption_other[] = {
    { 0x00, "Default" },
    /* all other reserved */
    { 0, NULL }
};

static const value_string pn_dcp_suboption_manuf[] = {
    /* none known */
    { 0, NULL }
};

/**
 * @brief The Dissector for Profi Real Time IO 0x8892.
 *
 */
struct BlockDissector {
  /** @brief Houses a Dissector internally for safe type casting. **/
  struct Dissector dissector;
};

void BlockDissector_free(Dissector_t *dissector);

int BlockDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops BlockDissectorOverride_ops = {

  sizeof(struct BlockDissector), /* size */
  (uint64_t) BLOCK_DISSECTOR_ID,
  (uint64_t) BLOCK_DISSECTOR_ID,
  BlockDissector_free,
  Dissector_registerSub,
  Dissector_getSub,
  BlockDissector_dissect
};

/**
 * @see Dissector_new
 */
Dissector_t *
BlockDissector_new() {


    struct BlockDissector *BlockDissector;

    BlockDissector = (struct BlockDissector *) Dissector_new(&BlockDissectorOverride_ops);
    check_mem(BlockDissector);

    return (Dissector_t*) BlockDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void
BlockDissector_free(Dissector_t *dissector) {

	struct BlockDissector *this = (struct BlockDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

/* dissect the "IP" suboption */
static int
dissect_PNDCP_Suboption_IP(tvbuff_t *tvb, int offset, packet_info *pinfo,
                            proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                            guint8 service_id, gboolean is_response)
{
    guint8      suboption;
    guint16     block_length;
    guint16     block_info;
    guint16     block_qualifier;
    guint32     ip;
    proto_item *item = NULL;
    address     addr;


    /* SuboptionIPParameter */
    offset = dissect_pn_uint8 (tvb, offset, pinfo, tree, hf_pn_dcp_suboption_ip, &suboption);
    /* DCPBlockLength */
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    switch (suboption) {
    case PNDCP_SUBOPTION_IP_MAC:
        /* MACAddressValue? */
        pn_append_info(pinfo, dcp_item, ", MAC");
        proto_item_append_text(block_item, "IP/MAC");

        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
        break;
    case PNDCP_SUBOPTION_IP_IP:
        pn_append_info(pinfo, dcp_item, ", IP");
        proto_item_append_text(block_item, "IP/IP");

        /* BlockInfo? */
        if ( ((service_id == PNDCP_SERVICE_ID_IDENTIFY) &&  is_response) ||
             ((service_id == PNDCP_SERVICE_ID_HELLO)    && !is_response) ||
             ((service_id == PNDCP_SERVICE_ID_GET)      &&  is_response)) {
            block_info = tvb_get_ntohs (tvb, offset);
            if (tree) {
                item = proto_tree_add_uint(tree, hf_pn_dcp_suboption_ip_block_info, tvb, offset, 2, block_info);
            }
            offset += 2;
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_suboption_ip_block_info, "Undecoded"));
            block_length -= 2;
            if (block_info & 0x80) {
                expert_add_info(pinfo, item, &ei_pn_dcp_ip_conflict);
            }
        }

        /* BlockQualifier? */
        if ( (service_id == PNDCP_SERVICE_ID_SET) && !is_response) {
            offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
            block_length -= 2;
        }

        /* IPParameterValue ... */

        /* IPAddress */
        offset = dissect_pn_ipv4(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_ip_ip, &ip);
        set_address(&addr, AT_IPv4, 4, &ip);
        proto_item_append_text(block_item, ", IP: %s", address_to_str(wmem_packet_scope(), &addr));

        /* Subnetmask */
        offset = dissect_pn_ipv4(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_ip_subnetmask, &ip);
        set_address(&addr, AT_IPv4, 4, &ip);
        proto_item_append_text(block_item, ", Subnet: %s", address_to_str(wmem_packet_scope(), &addr));

        /* StandardGateway */
        offset = dissect_pn_ipv4(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_ip_standard_gateway, &ip);
        set_address(&addr, AT_IPv4, 4, &ip);
        proto_item_append_text(block_item, ", Gateway: %s", address_to_str(wmem_packet_scope(), &addr));
        break;
    default:
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect the "device" suboption */
static int
dissect_PNDCP_Suboption_Device(tvbuff_t *tvb, int offset, packet_info *pinfo,
                               proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                               guint8 service_id, gboolean is_response)
{
    guint8    suboption;
    guint16   block_length;
    gchar    *info_str;
    guint8    device_role;
    guint16   vendor_id;
    guint16   device_id;
    char     *typeofstation;
    char     *nameofstation;
    char     *aliasname;
    guint16   block_info;
    guint16   block_qualifier;
    gboolean  have_block_info      = FALSE;
    gboolean  have_block_qualifier = FALSE;
    guint8    device_instance_high;
    guint8    device_instance_low;
    guint16   oem_vendor_id;
    guint16   oem_device_id;


    /* SuboptionDevice... */
    offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device, &suboption);
    /* DCPBlockLength */
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    /* BlockInfo? */
    if ( ((service_id == PNDCP_SERVICE_ID_IDENTIFY) &&  is_response) ||
         ((service_id == PNDCP_SERVICE_ID_HELLO)    && !is_response) ||
         ((service_id == PNDCP_SERVICE_ID_GET)      &&  is_response)) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_info, &block_info);
        have_block_info = TRUE;
        block_length -= 2;
    }

    /* BlockQualifier? */
    if ( (service_id == PNDCP_SERVICE_ID_SET) && !is_response) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        have_block_qualifier = TRUE;
        block_length -= 2;
    }

    switch (suboption) {
    case PNDCP_SUBOPTION_DEVICE_MANUF:
        typeofstation = (char *)wmem_alloc(wmem_packet_scope(), block_length+1);
        tvb_memcpy(tvb, (guint8 *) typeofstation, offset, block_length);
        typeofstation[block_length] = '\0';
        proto_tree_add_string (tree, hf_pn_dcp_suboption_device_typeofstation, tvb, offset, block_length, typeofstation);
        pn_append_info(pinfo, dcp_item, ", DeviceVendorValue");
        proto_item_append_text(block_item, "Device/Manufacturer specific");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info){
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", DeviceVendorValue: \"%s\"", typeofstation);
        offset += block_length;
        break;
    case PNDCP_SUBOPTION_DEVICE_NAMEOFSTATION:
        nameofstation = (char *)wmem_alloc(wmem_packet_scope(), block_length+1);
        tvb_memcpy(tvb, (guint8 *) nameofstation, offset, block_length);
        nameofstation[block_length] = '\0';
        proto_tree_add_string (tree, hf_pn_dcp_suboption_device_nameofstation, tvb, offset, block_length, nameofstation);
        pn_append_info(pinfo, dcp_item, wmem_strdup_printf(wmem_packet_scope(), ", NameOfStation:\"%s\"", nameofstation));
        proto_item_append_text(block_item, "Device/NameOfStation");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", \"%s\"", nameofstation);
        offset += block_length;
        break;
    case PNDCP_SUBOPTION_DEVICE_DEV_ID:
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_vendor_id, &vendor_id);
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_id, &device_id);
        pn_append_info(pinfo, dcp_item, ", Dev-ID");
        proto_item_append_text(block_item, "Device/Device ID");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", VendorID: 0x%04x / DeviceID: 0x%04x", vendor_id, device_id);
        break;
    case PNDCP_SUBOPTION_DEVICE_DEV_ROLE:
        offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_role, &device_role);
        offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_reserved8, NULL);
        pn_append_info(pinfo, dcp_item, ", Dev-Role");
        proto_item_append_text(block_item, "Device/Device Role");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info)
            proto_item_append_text(block_item, ", BlockInfo: %s", val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        if (device_role & 0x01)
            proto_item_append_text(block_item, ", IO-Device");
        if (device_role & 0x02)
            proto_item_append_text(block_item, ", IO-Controller");
        if (device_role & 0x04)
            proto_item_append_text(block_item, ", IO-Multidevice");
        if (device_role & 0x08)
            proto_item_append_text(block_item, ", PN-Supervisor");
        break;
    case PNDCP_SUBOPTION_DEVICE_DEV_OPTIONS:
        info_str = wmem_strdup_printf(wmem_packet_scope(), ", Dev-Options(%u)", block_length/2);
        pn_append_info(pinfo, dcp_item, info_str);
        proto_item_append_text(block_item, "Device/Device Options");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", %u options", block_length/2);
        for( ; block_length != 0; block_length -= 2) {
            offset = dissect_PNDCP_Option(tvb, offset, pinfo, tree, NULL /*block_item*/, hf_pn_dcp_option,
                FALSE /* append_col */);
        }
        break;
    case PNDCP_SUBOPTION_DEVICE_ALIAS_NAME:
        aliasname = (char *)wmem_alloc(wmem_packet_scope(), block_length+1);
        tvb_memcpy(tvb, (guint8 *) aliasname, offset, block_length);
        aliasname[block_length] = '\0';
        proto_tree_add_string (tree, hf_pn_dcp_suboption_device_aliasname, tvb, offset, block_length, aliasname);
        pn_append_info(pinfo, dcp_item, wmem_strdup_printf(wmem_packet_scope(), ", AliasName:\"%s\"", aliasname));
        proto_item_append_text(block_item, "Device/AliasName");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", \"%s\"", aliasname);
        offset += block_length;
        break;
    case PNDCP_SUBOPTION_DEVICE_DEV_INSTANCE:
        offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_instance_high, &device_instance_high);
        offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_instance_low, &device_instance_low);
        pn_append_info(pinfo, dcp_item, ", Dev-Instance");
        proto_item_append_text(block_item, "Device/Device Instance");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", InstanceHigh: %d, Instance Low: %d",
                               device_instance_high, device_instance_low);
        break;
    case PNDCP_SUBOPTION_DEVICE_OEM_DEV_ID:
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_oem_ven_id, &oem_vendor_id);
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_device_oem_dev_id, &oem_device_id);
        pn_append_info(pinfo, dcp_item, ", OEM-Dev-ID");
        proto_item_append_text(block_item, "Device/OEM Device ID");
        if(have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if(have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_item_append_text(block_item, ", OEMVendorID: 0x%04x / OEMDeviceID: 0x%04x", oem_vendor_id, oem_device_id);
        break;
    default:
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect the "DHCP" suboption */
static int
dissect_PNDCP_Suboption_DHCP(tvbuff_t *tvb, int offset, packet_info *pinfo,
                                proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                                guint8 service_id _U_, gboolean is_response _U_)
{
    guint8   suboption;
    guint16  block_length;
    guint16  block_info;
    guint16  block_qualifier;
    gboolean have_block_info      = FALSE;
    gboolean have_block_qualifier = FALSE;


    offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_dhcp, &suboption);
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    /* BlockInfo? */
    if ( ((service_id == PNDCP_SERVICE_ID_IDENTIFY) &&  is_response) ||
         ((service_id == PNDCP_SERVICE_ID_HELLO)    && !is_response) ||
         ((service_id == PNDCP_SERVICE_ID_GET)      &&  is_response)) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_info, &block_info);
        have_block_info=TRUE;
        block_length -= 2;
    }

    /* BlockQualifier? */
    if ( (service_id == PNDCP_SERVICE_ID_SET) && !is_response) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        have_block_qualifier=TRUE;
        block_length -= 2;
    }

    switch (suboption) {
    case PNDCP_SUBOPTION_DHCP_CLIENT_ID:
        pn_append_info(pinfo, dcp_item, ", DHCP client identifier");
        proto_item_append_text(block_item, "DHCP/Client-ID");
        if (have_block_qualifier) {
            proto_item_append_text(block_item, ", BlockQualifier: %s",
                                   val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        }
        if (have_block_info) {
            proto_item_append_text(block_item, ", BlockInfo: %s",
                                   val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        }
        proto_tree_add_item(tree, hf_pn_dcp_suboption_dhcp_device_id, tvb, offset, block_length, ENC_NA);
        offset += block_length;
        break;
    default:
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect the "control" suboption */
static int
dissect_PNDCP_Suboption_Control(tvbuff_t *tvb, int offset, packet_info *pinfo,
                                proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                                guint8 service_id _U_, gboolean is_response _U_)
{
    guint8      suboption;
    guint16     block_length;
    guint16     block_qualifier;
    guint16     BlockQualifier;
    gchar      *info_str;
    guint8      block_error;
    proto_item *item = NULL;


    offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_control, &suboption);
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    switch (suboption) {
    case PNDCP_SUBOPTION_CONTROL_START_TRANS:
        pn_append_info(pinfo, dcp_item, ", Start-Trans");
        proto_item_append_text(block_item, "Control/Start-Transaction");
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        break;
    case PNDCP_SUBOPTION_CONTROL_END_TRANS:
        pn_append_info(pinfo, dcp_item, ", End-Trans");
        proto_item_append_text(block_item, "Control/End-Transaction");
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        break;
    case PNDCP_SUBOPTION_CONTROL_SIGNAL:
        pn_append_info(pinfo, dcp_item, ", Signal");
        proto_item_append_text(block_item, "Control/Signal");
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        block_length -= 2;

        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
        break;
    case PNDCP_SUBOPTION_CONTROL_RESPONSE:
        proto_item_append_text(block_item, "Control/Response");
        offset = dissect_PNDCP_Option(tvb, offset, pinfo, tree, block_item, hf_pn_dcp_suboption_control_response,
            FALSE /* append_col */);
        block_error = tvb_get_guint8 (tvb, offset);
        if (tree) {
            item = proto_tree_add_uint(tree, hf_pn_dcp_block_error, tvb, offset, 1, block_error);
        }
        offset += 1;
        if (block_error != 0) {
            expert_add_info_format(pinfo, item, &ei_pn_dcp_block_error_unknown, "%s",
                                    val_to_str(block_error, pn_dcp_block_error, "Unknown"));
        }
        info_str = wmem_strdup_printf(wmem_packet_scope(), ", Response(%s)",
                                      val_to_str(block_error, pn_dcp_block_error, "Unknown"));
        pn_append_info(pinfo, dcp_item, info_str);
        proto_item_append_text(block_item, ", BlockError: %s",
                                    val_to_str(block_error, pn_dcp_block_error, "Unknown"));

        break;
    case PNDCP_SUBOPTION_CONTROL_FACT_RESET:
        pn_append_info(pinfo, dcp_item, ", Reset FactorySettings");
        proto_item_append_text(block_item, "Control/Reset FactorySettings");
        block_length -= 2;
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_blockqualifier_r2f, &BlockQualifier);
        proto_item_append_text(block_item, ", BlockQualifier: %s",
            val_to_str(BlockQualifier, pn_dcp_suboption_other, "reserved"));
        block_length -= 2;
        break;

    case PNDCP_SUBOPTION_CONTROL_RESET_TO_FACT:
        pn_append_info(pinfo, dcp_item, ", Reset to Factory");
        proto_item_append_text(block_item, "Reset to FactorySettings");

        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_blockqualifier, &BlockQualifier);
        proto_item_append_text(block_item, ", BlockQualifier: %s",
            val_to_str(BlockQualifier, pn_dcp_BlockQualifier, "reserved"));
        block_length -= 2;

        break;
    default:
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect the "deviceinitaitve" suboption */
static int
dissect_PNDCP_Suboption_DeviceInitiative(tvbuff_t *tvb, int offset, packet_info *pinfo,
                            proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                            guint8 service_id, gboolean is_response)
{
    guint8  suboption;
    guint16 block_length;
    guint16 block_info;
    guint16 block_qualifier;
    guint16 value;


    offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_deviceinitiative, &suboption);
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    pn_append_info(pinfo, dcp_item, ", DeviceInitiative");
    proto_item_append_text(block_item, "DeviceInitiative/DeviceInitiative");

    /* BlockInfo? */
    if ( ((service_id == PNDCP_SERVICE_ID_IDENTIFY) &&  is_response) ||
        ((service_id == PNDCP_SERVICE_ID_HELLO)    && !is_response) ||
        ((service_id == PNDCP_SERVICE_ID_GET)      &&  is_response)) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_info, &block_info);
        proto_item_append_text(block_item, ", BlockInfo: %s",
                               val_to_str(block_info, pn_dcp_block_info, "Unknown"));
        block_length -= 2;
    }

    /* BlockQualifier? */
    if ( (service_id == PNDCP_SERVICE_ID_SET) && !is_response) {
        offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_qualifier, &block_qualifier);
        proto_item_append_text(block_item, ", BlockQualifier: %s",
                               val_to_str(block_qualifier, pn_dcp_block_qualifier, "Unknown"));
        block_length -= 2;
    }

    /* DeviceInitiativeValue */
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_deviceinitiative_value, &value);

    return offset;
}


/* dissect the "all" suboption */
static int
dissect_PNDCP_Suboption_All(tvbuff_t *tvb, int offset, packet_info *pinfo,
                            proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                            guint8 service_id _U_, gboolean is_response _U_)
{
    guint8  suboption;
    guint16 block_length;


    offset = dissect_pn_uint8(tvb, offset, pinfo, tree, hf_pn_dcp_suboption_all, &suboption);
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length, &block_length);

    switch (suboption) {
    case 255:    /* All */
        pn_append_info(pinfo, dcp_item, ", All");
        proto_item_append_text(block_item, "All/All");
        break;
    default:
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect the "manufacturer" suboption */
static int
dissect_PNDCP_Suboption_Manuf(tvbuff_t *tvb, int offset, packet_info *pinfo,
                            proto_tree *tree, proto_item *block_item, proto_item *dcp_item,
                            guint8 service_id _U_, gboolean is_response _U_)
{
    guint8  suboption;
    guint16 block_length;


    offset = dissect_pn_uint8( tvb, offset, pinfo, tree, hf_pn_dcp_suboption_manuf, &suboption);
    offset = dissect_pn_uint16(tvb, offset, pinfo, tree, hf_pn_dcp_block_length,    &block_length);

    switch (suboption) {
    default:
        pn_append_info(pinfo, dcp_item, ", Manufacturer Specific");
        proto_item_append_text(block_item, "Manufacturer Specific");
        offset = dissect_pn_undecoded(tvb, offset, pinfo, tree, block_length);
    }

    return offset;
}


/* dissect one DCP block */
static int
dissect_PNDCP_Block(tvbuff_t *tvb, int offset, packet_info *pinfo,
                    proto_tree *tree, proto_item *dcp_item,
                    guint8 service_id, gboolean is_response)
{
    guint8      option;
    proto_item *block_item;
    proto_tree *block_tree;
    int         ori_offset = offset;

    /* subtree for block */
    block_item = proto_tree_add_none_format(tree, hf_pn_dcp_block,
        tvb, offset, 0, "Block: ");
    block_tree = proto_item_add_subtree(block_item, ett_pn_dcp_block);


    offset = dissect_pn_uint8(tvb, offset, pinfo, block_tree, hf_pn_dcp_option, &option);

    switch (option) {
    case PNDCP_OPTION_IP:
        offset = dissect_PNDCP_Suboption_IP(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DEVICE:
        offset = dissect_PNDCP_Suboption_Device(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DHCP:
        offset = dissect_PNDCP_Suboption_DHCP(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_CONTROL:
        offset = dissect_PNDCP_Suboption_Control(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DEVICEINITIATIVE:
        offset = dissect_PNDCP_Suboption_DeviceInitiative(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_ALLSELECTOR:
        offset = dissect_PNDCP_Suboption_All(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_MANUF_X80:
    case PNDCP_OPTION_MANUF_X81:
    default:
        offset = dissect_PNDCP_Suboption_Manuf(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
    }

    proto_item_set_len(block_item, offset-ori_offset);

    if ((offset-ori_offset) & 1) {
        /* we have an odd number of bytes in this block, add a padding byte */
        offset = dissect_pn_padding(tvb, offset, pinfo, tree, 1);
    }

    return offset;
}

/**
 * @see Dissector_dissect
 */
int BlockDissector_dissect(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {

    uint8_t      option;
    proto_item *block_item;
    proto_tree *block_tree;
    int         ori_offset = offset;

    /* subtree for block */
    block_item = proto_tree_add_none_format(tree, hf_pn_dcp_block,
        tvb, offset, 0, "Block: ");
    block_tree = proto_item_add_subtree(block_item, ett_pn_dcp_block);


    offset = dissect_pn_uint8(tvb, offset, pinfo, block_tree, hf_pn_dcp_option, &option);

    switch (option) {
    case PNDCP_OPTION_IP:
        offset = dissect_PNDCP_Suboption_IP(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DEVICE:
        offset = dissect_PNDCP_Suboption_Device(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DHCP:
        offset = dissect_PNDCP_Suboption_DHCP(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_CONTROL:
        offset = dissect_PNDCP_Suboption_Control(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_DEVICEINITIATIVE:
        offset = dissect_PNDCP_Suboption_DeviceInitiative(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_ALLSELECTOR:
        offset = dissect_PNDCP_Suboption_All(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
        break;
    case PNDCP_OPTION_MANUF_X80:
    case PNDCP_OPTION_MANUF_X81:
    default:
        offset = dissect_PNDCP_Suboption_Manuf(tvb, offset, pinfo, block_tree, block_item, dcp_item, service_id, is_response);
    }

    proto_item_set_len(block_item, offset-ori_offset);

    if ((offset-ori_offset) & 1) {
        /* we have an odd number of bytes in this block, add a padding byte */
        offset = dissect_pn_padding(tvb, offset, pinfo, tree, 1);
    }

    return offset;

error:
	return -1;
}
