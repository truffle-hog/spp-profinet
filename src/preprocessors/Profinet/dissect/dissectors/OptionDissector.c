/**
 * @file
 * @brief OptionDissector implementation.
 *
 * This Dissector dissects pn blocks
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/dissectors/OptionDissector.h"

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
struct OptionDissector {
  /** @brief Houses a Dissector internally for safe type casting. **/
  struct Dissector dissector;
  bool isResponse;
};

void OptionDissector_free(Dissector_t *dissector);

int OptionDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops OptionDissectorOverride_ops = {

  sizeof(struct OptionDissector), /* size */
  (uint64_t) OPTION_DISSECTOR_ID,
  (uint64_t) OPTION_DISSECTOR_ID,
  OptionDissector_free,
  Dissector_registerSub,
  Dissector_getSub,
  OptionDissector_dissect
};

/**
 * @see Dissector_new
 */
Dissector_t *
OptionDissector_new() {


    struct OptionDissector *OptionDissector;

    OptionDissector = (struct OptionDissector *) Dissector_new(&OptionDissectorOverride_ops);
    check_mem(OptionDissector);

    return (Dissector_t*) OptionDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void
OptionDissector_free(Dissector_t *dissector) {

	struct OptionDissector *this = (struct OptionDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

/**
 * @see Dissector_dissect
 */
int OptionDissector_dissect(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {

	(void) this;
	(void) buf;
	(void) node;

	int bytesDissected = 0;

    const value_string *val_str;

	struct Value option;
	option.type = is_uint8;
	option.val.uint8 = buf->ops->Buffy_getBits8(buf, bytesDissected++);
	option.length = 8;

	struct Value suboption;
	suboption.type = is_uint8;
	suboption.val.uint8 = buf->ops->Buffy_getBits8(buf, bytesDissected++);
	suboption.length = 8;

	ProtocolItem_t *optionItem;

    switch (option.val.uint8) {
    case PNDCP_OPTION_IP:

		optionItem = node->ops->ProtocolTree_branch(node, "options.ip_option", option);
		val_str = pn_dcp_suboption_ip;
        break;
    case PNDCP_OPTION_DEVICE:

		optionItem = node->ops->ProtocolTree_branch(node, "options.device_option", option);
        val_str = pn_dcp_suboption_device;
        break;
    case PNDCP_OPTION_DHCP:

		optionItem = node->ops->ProtocolTree_branch(node, "options.dhcp_option", option);
        val_str = pn_dcp_suboption_dhcp;
        break;
    case PNDCP_OPTION_CONTROL:
        optionItem = node->ops->ProtocolTree_branch(node, "options.control_option", option);
        val_str = pn_dcp_suboption_control;
        break;
    case PNDCP_OPTION_DEVICEINITIATIVE:

		optionItem = node->ops->ProtocolTree_branch(node, "options.deviceinitiative_option", option);
        val_str = pn_dcp_suboption_deviceinitiative;
        break;
    case PNDCP_OPTION_ALLSELECTOR:

		optionItem = node->ops->ProtocolTree_branch(node, "options.allselector_option", option);
        val_str = pn_dcp_suboption_all;
        break;
    default:
		optionItem = node->ops->ProtocolTree_branch(node, "options.manufacturer_option", option);
        val_str = pn_dcp_suboption_manuf;
    }

	check_mem(optionItem);

	struct Value suboptionValue;
	suboptionValue.type = is_string;

	suboptionValue.val.string = valueToString(suboption.val.uint8, val_str, "Unknown");

	ProtocolItem_t *suboptionItem = optionItem->ops->ProtocolTree_branch(optionItem, "suboption", suboptionValue);
	check_mem(suboptionItem);

	return bytesDissected;
	/*
    proto_item_append_text(block_item, ", Status from %s - %s",
        val_to_str(option, pn_dcp_option, "Unknown"), val_to_str(suboption, val_str, "Unknown"));

    if (append_col) {
        col_append_fstr(pinfo->cinfo, COL_INFO, ", %s", val_to_str(suboption, val_str, "Unknown"));
    }

    return offset;
*/
	/*
    uint16_t u16FrameID = buf->ops->Buffy_getBits16(buf, 0);
    check (u16FrameID >= FRAME_ID_DCP_HELLO || u16FrameID <= FRAME_ID_DCP_IDENT_RES, "no valid frame id for dcp");

    struct Value dcp;
	dcp.type = is_string;
	dcp.val.string = "DCP";

    ProtocolItem_t *dcpItem = node->ops->ProtocolTree_branch(node, "dcp", dcp);

    return _dissectPNDCP_PDU(this, buf->ops->Buffy_createVirtual(buf, 16), dcpItem);

	*/
error:
	return -1;
}
