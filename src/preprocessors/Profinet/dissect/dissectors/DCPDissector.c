/**
 * @file
 * @brief DCPDissetor implementation.
 *
 * This Dissector is the 0x8892 toplevel dissector, which will be followed by frame
 * and block dissectors.
 *
 */
#include <stdlib.h>
#include <stdio.h>

#include "dissect/Dissector-int.h"
#include "dissect/Dissector.h"
#include "dissect/dissectors/DCPDissector.h"

// subdissectors
#include "dissect/dissectors/BlockDissector.h"
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
struct DCPDissector {
  /** @brief Houses a Dissector internally for safe type casting. **/
  struct Dissector dissector;
};

void DCPDissector_free(Dissector_t *dissector);

int DCPDissector_dissect(Dissector_t *this, Buffy_t *buf, ProtocolTree_t *tree);

/**
 * @see Dissector_ops
 */
static const struct Dissector_ops DCPDissectorOverride_ops = {

  sizeof(struct DCPDissector), /* size */
  (uint64_t) FRAME_ID_DCP_HELLO,
  (uint64_t) FRAME_ID_DCP_IDENT_RES,
  DCPDissector_free,
  Dissector_registerSub,
  Dissector_getSub,
  DCPDissector_dissect,
  "DCP Dissector",
  0x4
};

int DCPDissector_initializeSubDissectors(Dissector_t *this) {

    Dissector_t *blockDissector = BlockDissector_new();
    check_mem(blockDissector); // TODO check that this check is done everywhere

    this->ops->Dissector_registerSub(this, blockDissector);

    Dissector_t *optionDissector = OptionDissector_new();
    check_mem(optionDissector); // TODO check that this check is done everywhere

    this->ops->Dissector_registerSub(this, optionDissector);

    return 0;

error:
    return -1;

}

/**
 * @see Dissector_new
 */
Dissector_t *
DCPDissector_new() {


    struct DCPDissector *dcpDissector;

    dcpDissector = (struct DCPDissector *) Dissector_new(&DCPDissectorOverride_ops);
    check_mem(dcpDissector);

    check(
        DCPDissector_initializeSubDissectors(&dcpDissector->dissector) == 0,
        "the subdissectors could not be registered successfully");

    return (Dissector_t*) dcpDissector;
    // dcpDissector = malloc(sizeof(struct DCPDissector));
    // check_mem(dcpDissector);
    //
    // Dissector_t *dissector = Dissector_new(&DCPDissectorOverride_ops);
    // check_mem(dissector);
    //
    // dcpDissector->dissector = dissector;
    //
    // return (Dissector_t*) dcpDissector;

error:
    return NULL;

}

/**
 * @see Dissector_free
 */
void
DCPDissector_free(Dissector_t *dissector) {

	struct DCPDissector *this = (struct DCPDissector*) dissector;

	free(this);
//	this->dissector.ops->Dissector_free(&this->dissector);

	(void) dissector;
	// TODO implement
}

static int
_dissectPNDCP_PDU(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {



    check(this != NULL, "dissector must not be null");
    check(buf != NULL, "buffer must not be null");
    check(node != NULL, "node must not be null");

    int offset = 0;
	int dataLength;

	struct Value serviceID;
	serviceID.type = is_uint8;
	serviceID.length = 8;

	struct Value serviceType;
	serviceType.type = is_uint8;
	serviceType.length = 8;

	struct Value xid;
	xid.type = is_uint32;
	xid.length = 32;

	struct Value responseDelay;
	responseDelay.type = is_uint16;
	responseDelay.length = 16;




//    char *xidStr;
    bool isResponse = FALSE;

    serviceID.val.uint8 = buf->ops->Buffy_getBits8(buf, offset++);
    serviceType.val.uint8 = buf->ops->Buffy_getBits8(buf, offset++);
	xid.val.uint32 = buf->ops->Buffy_getBits32(buf, offset);
	offset += 4;

    ProtocolItem_t *serviceIDItem = node->ops->ProtocolTree_branchImportant(node, "service_id", "dcp_service_id", serviceID, this);
	check_mem(serviceIDItem);

    ProtocolItem_t *serviceTypeItem = node->ops->ProtocolTree_branchImportant(node, "service_type", "dcp_service_type", serviceType, this);
	check_mem(serviceTypeItem);

    ProtocolItem_t *xidItem = node->ops->ProtocolTree_branchImportant(node, "xid", "dcp_xid", xid, this);
	check_mem(xidItem);

	if (serviceID.val.uint8 == PNDCP_SERVICE_ID_IDENTIFY && serviceType.val.uint8 == PNDCP_SERVICE_TYPE_REQUEST) {

		/* multicast header */
		responseDelay.val.uint16 = buf->ops->Buffy_getBits16(buf, offset);

        ProtocolItem_t *responseDelayItem = node->ops->ProtocolTree_branchImportant(node, "response_delay", "dcp_response_delay", responseDelay, this);
    	check_mem(responseDelayItem);
		offset += 2;
	} else {
		/* unicast header -- just 16 reserved bits*/
		offset += 2;
	}

    struct Value dataLengthValue;
    dataLengthValue.type = is_uint16;
    dataLengthValue.val.uint16 = buf->ops->Buffy_getBits16(buf, offset);
    dataLengthValue.length = 16;


	dataLength = (int) dataLengthValue.val.uint16;
    offset += 2;

    ProtocolItem_t *dataLengthItem = node->ops->ProtocolTree_branchImportant(node, "data_length", "dcp_data_length", dataLengthValue, this);
    check_mem(dataLengthItem);



	struct Value serviceIDName;
	serviceIDName.type = is_string;

	switch (serviceID.val.uint8) {
	case PNDCP_SERVICE_ID_GET:
		serviceIDName.val.string = "Get";
		break;

	case PNDCP_SERVICE_ID_SET:
		serviceIDName.val.string = "Set";
		break;

	case PNDCP_SERVICE_ID_IDENTIFY:
		serviceIDName.val.string = "Identify";
		break;

	case PNDCP_SERVICE_ID_HELLO:
		serviceIDName.val.string = "Hello";
		break;

	default:
		// TODO potential alarm / malformed??
		serviceIDName.val.string = "Unknown";
		break;
	}

	ProtocolItem_t *serviceIDStringItem = serviceIDItem->ops->ProtocolTree_branchImportant(serviceIDItem, "name", "service_id_name", serviceIDName, this);

	check_mem(serviceIDStringItem);

    struct Value isResponseValue;
    isResponseValue.type = is_uint8;
    isResponseValue.val.uint8 = 0;

	struct Value serviceTypeName;
	serviceTypeName.type = is_string;

	switch (serviceType.val.uint8) {
	case PNDCP_SERVICE_TYPE_REQUEST:
		serviceTypeName.val.string = "Request";
		break;

	case PNDCP_SERVICE_TYPE_RESPONSE_SUCCESS:
		serviceTypeName.val.string = "Resonse OK";
        isResponse = TRUE;
        isResponseValue.val.uint8 = 1;
		break;

	case PNDCP_SERVICE_TYPE_RESPONSE_UNSUPPORTED:
		serviceTypeName.val.string = "Response Unsupported";
        isResponse = TRUE;
        isResponseValue.val.uint8 = 1;
		break;

	default:
		// TODO potential alarm / malformed??
		serviceTypeName.val.string = "Unknown";
		break;
	}

    ProtocolItem_t *isResponseItem = node->ops->ProtocolTree_branchImportant(node, "is_response", "dcp_is_response", isResponseValue, this);
    check_mem(isResponseItem);

	ProtocolItem_t *serviceTypeNameItem = serviceTypeItem->ops->ProtocolTree_branchImportant(serviceTypeItem, "name", "service_type_name", serviceTypeName, this);
	check_mem(serviceTypeNameItem);

	struct Value options;
	options.type = is_string;
	options.val.string = "Options";

	struct Value blocks;
	blocks.type = is_string;
	blocks.val.string = "Blocks";

	ProtocolItem_t *optionsItem = node->ops->ProtocolTree_branch(node, "options", options, this);
	check_mem(optionsItem);

	ProtocolItem_t *blocksItem = node->ops->ProtocolTree_branch(node, "blocks", blocks, this);
	check_mem(blocksItem);

	Buffy_t *loopBuffer; // = buf->ops->Buffy_createVirtual(buf, offset);

	debug("datalength before blocks and options: %d", dataLength);

	while (dataLength) {

		loopBuffer = buf->ops->Buffy_createVirtual(buf, offset * 8);
		check_mem(loopBuffer);

		int originalOffset = offset;
        int bytesDissected = 0;

		Dissector_t *nextDissector;

		if (serviceID.val.uint8 == PNDCP_SERVICE_ID_GET && serviceType.val.uint8 == PNDCP_SERVICE_TYPE_REQUEST) {

			nextDissector = this->ops->Dissector_getSub(this, OPTION_DISSECTOR_ID);
            check(nextDissector != NULL, "there exists not subdissector for the dcp options");
            //((struct OptionDissector *) nextDissector)->isResponse = isResponse;
            //((struct OptionDissector *) nextDissector)->serviceID = serviceID;

            bytesDissected = nextDissector->ops->Dissector_dissect(nextDissector, loopBuffer, optionsItem);

		} else {

			nextDissector = this->ops->Dissector_getSub(this, BLOCK_DISSECTOR_ID);
            check(nextDissector != NULL, "there exists not subdissector for the dcp blocks");

			struct BlockDissector *blockDissector = (struct BlockDissector *) nextDissector;
			blockDissector->isResponse = isResponse;
			blockDissector->serviceID = serviceID.val.uint8;
			//((struct BlockDissector *) nextDissector)->isResponse = isResponse;
            //((struct BlockDissector *) nextDissector)->serviceID = serviceID;

            bytesDissected = nextDissector->ops->Dissector_dissect(nextDissector, loopBuffer, blocksItem);
		}

		debug("bytes Dissected: %d", bytesDissected);

		check (bytesDissected >= 0, "dissecting the block or option failed");

		offset += bytesDissected;

		loopBuffer->ops->Buffy_free(loopBuffer);

        // check(dataLength > 0)
        //
		// check(offset > originalOffset || dataLength >= (offset - originalOffset), "Bounds Error");

		dataLength -= bytesDissected;

        debug("current datalength: %d", dataLength);

        if (dataLength <= 0) {
            break;
        }
	}

    return offset;

	//struct Value xidName;
	//xidName.type = is_string;
	//xidName.val.string = "Xid:0x%x", xid.val.uint32;


	//ProtocolItem_t *xidStringItem = xidItem->ops->ProtocolTree_branch(xidItem, "name", xid



  //  debug("%d", serviceID);
  //  debug("%d", serviceType);

error:

    return -1;
}

/**
 * @see Dissector_dissect
 */
int DCPDissector_dissect(Dissector_t *this, Buffy_t *buf, struct ProtocolNode *node) {

	(void) this;
	(void) buf;
	(void) node;

    uint16_t u16FrameID = buf->ops->Buffy_getBits16(buf, 0);
    check (u16FrameID >= FRAME_ID_DCP_HELLO || u16FrameID <= FRAME_ID_DCP_IDENT_RES, "no valid frame id for dcp");

    struct Value dcp;
	dcp.type = is_string;
	dcp.val.string = "DCP";

    ProtocolItem_t *dcpItem = node->ops->ProtocolTree_branch(node, "dcp", dcp, this);

    int dissected;

    check(
        !((dissected = _dissectPNDCP_PDU(this, buf->ops->Buffy_createVirtual(buf, 16), dcpItem)) < 0), "error on dissection in DCP");

    return dissected + 2;

error:
	return -1;
}
