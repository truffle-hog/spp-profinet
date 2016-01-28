/* $Id$ */
/* Snort Preprocessor Plugin Source File ProfiNet */

/* spp_profinet_decode
 *
 * Purpose:
 *
 * Preprocessors perform some function *once* for *each* packet.  This is
 * different from detection plugins, which are accessed depending on the
 * standard rules.  When adding a plugin to the system, be sure to
 * add the "Setup" function to the InitPreprocessors() function call in
 * plugbase.c!
 *
 * Arguments:
 *
 * This is the list of arguements that the plugin can take at the
 * "preprocessor" line in the rules file
 *
 * Effect:
 *
 * What the preprocessor does.  Check out some of the default ones
 * (e.g. spp_frag2) for a good example of this description.
 *
 * Comments:
 *
 * Any comments?
 *
 */

#include <sys/types.h>
//#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <rpc/types.h>

/*
 * If you're going to issue any alerts from this preproc you
 * should include generators.h and event_wrapper.h
 */
#include "generators.h"
#include "event_wrapper.h"

#include "util.h"
#include "plugbase.h"
#include "parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <pwd.h>
#include <pthread.h>
#include "dbg.h"
//#include "proto.h"

//#include "Profinet/plugin.c"

//#include "Profinet/epan/tvbuff.h"
//#include "Profinet/epan/tvbuff-int.h"
//#include <glib.h>

/*
 * put in other inculdes as necessary
 */

/*
 * your preprocessor header file goes here if necessary, don't forget
 * to include the header file in plugbase.h too!
 */
#include "spp_profinet.h"

#define PROFI_ETHER_TYPE    0x8892

#define FRAME_ID_RT_DCP_REQ	0xfefe
#define FRAME_ID_RT_DCP_RES	0xfeff

#define FRAME_ID_RT_C1		0xc000 // -> 0xf7ff

typedef struct _ProfiNet
{
	uint16_t frameID;

} ProfiNet;

/*
typdef struct _FrameRtDcpRequest
{



}*/


/*
 * define any needed data structs for things like configuration
 */
typedef struct _Truffle
{
	struct timeval time_stamp;

	uint32_t caplen; /* length of the portion presen */
	uint32_t pktlen; /* length of the whole packet (incl off wire) */

	uint64_t ether_dst;
	uint64_t ether_src;
	uint16_t ether_type;

	uint16_t frameID;

    /* Your struct members here */
} Truffle;

typedef struct _SocketData
{
	int server_sockfd;
	int client_sockfd;

	int client_detected;

	struct sockaddr_un serv_addr;
	struct sockaddr_un cli_addr;
	socklen_t clilen;

} SocketData;

/*
 * If you need to instantiate the preprocessor's
 * data structure, do it here
 */
// Truffle SomeTruffle;

//const char *homedir;

int sockfd = -1, newsockfd = -1;

int client_detected = 0;

SocketData* data;
/*
 * function prototypes go here
 */

static void ProfiNetInit(struct _SnortConfig *, char *args);
//static void ProfiNetClientInit(struct _SnortConfig *, char *args);

//static void ParseProfiNetArgs(char *);
//static void ParseProfiNetClientArgs(char *);

static void DetectProfiNetPackets(Packet *, void *context);
static void ProfiNetCleanExit(int, void *);
//static void PreprocRestartFunction(int, void *);


#ifdef SNORT_RELOAD
static void ProfiNetReload(struct _SnortConfig *, char *, void **);
//static void ProfiNetReloadClient(struct _SnortConfig *, char *, void **);
static void * ProfiNetReloadSwap(struct _SnortConfig *, void *);
static void ProfiNetReloadSwapFree(void *);
#endif

/*
 * Function: SetupProfiNet()
 *
 * Purpose: Registers the preprocessor keyword and initialization
 *          function into the preprocessor list.  This is the function that
 *          gets called from InitPreprocessors() in plugbase.c.
 *
 * Arguments: None.
 *
 * Returns: void function
 *
 */
void SetupProfiNet()
{
#ifndef SNORT_RELOAD
	 /*
     * link the preprocessor keyword to the init function in
     * the preproc list
     */
    RegisterPreprocessor("profinet", ProfiNetInit);
//	RegisterPreprocessor("profinet_detect_client", ProfiNetClientInit);

#else
	RegisterPreprocessor("profinet", ProfiNetInit, ProfiNetReload, NULL,
						ProfiNetReloadSwap, ProfiNetReloadSwapFree);
//	RegisterPreprocessor("profinet_detect_client", ProfiNetClientInit,
//						ProfiNetReloadClient, NULL, NULL, NULL);

#endif

    DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Preprocessor: ProfiNet is setup...\n"));
}

char * get_socket_file() {

	char * socket_name = "/socket.sock";
	char * homedir;

	if ((homedir = getenv("HOME")) == NULL) {

		homedir = getpwuid(getuid())->pw_dir;
	}
	char *socket_file = malloc(strlen(homedir) + strlen(socket_name) + 1);
	check_mem(socket_file);

	socket_file[0] = '\0';
	strcat(socket_file, homedir);
	strcat(socket_file, socket_name);

	return socket_file;

error:
	return NULL;
}

void * await_request( void* args) {

	(void) args;
//	SocketData *sockData = (SocketData*) args;
	int n;
	char buffer[256];
	data->client_sockfd = accept (data->server_sockfd, (struct sockaddr *) &(data->cli_addr), &(data->clilen));
	check(data->client_sockfd >= 0, "error on accept");


	memset(buffer, 0, 256);

	n = read(data->client_sockfd,buffer,255);
	check(n >= 0, "error reading from socket");

	DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Received message: %s\n",buffer));

	data->client_detected = 1;

	return NULL;
error:
	// TODO error handling
	return NULL;

}

/*
 * Function: ProfiNetInit(u_char *)
 *
 * Purpose: Calls the argument parsing function, performs final setup on data
 *          structs, links the preproc function into the function list.
 *
 * Arguments: args => ptr to argument string
 *
 * Returns: void function
 *
 */
static void ProfiNetInit(struct _SnortConfig * sc, char *args)
{
	(void) args;

	/* register the profinet plugins from wireshark */

//	plugin_register();
//	plugin_reg_handoff();


//	printf("initialize ProfiNet preprocessor\n");
	data = malloc (sizeof(SocketData));
	check_mem(data);
	//	SocketData *sockData = malloc (sizeof(SocketData));
	//	check_mem(sockData);
	// declare some variabled in use
	pthread_t thread;
	int bind_check, len;

	// open the unix socket, as sock_stream
	data->server_sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	check(data->server_sockfd != -1, "cannot open socket");

	memset((char *) &(data->serv_addr), 0, sizeof(data->serv_addr));
	char * socket_file = get_socket_file();
	check_mem(socket_file);

	data->serv_addr.sun_family = AF_UNIX;
	strcpy(data->serv_addr.sun_path, socket_file);
	len = strlen(data->serv_addr.sun_path) + sizeof(data->serv_addr.sun_family);

	unlink(socket_file);
	bind_check = bind(data->server_sockfd, (struct sockaddr *) &(data->serv_addr), len);
	check(bind_check != -1, "error on binding");

	listen(data->server_sockfd, 5);
	data->clilen = sizeof(data->cli_addr);
	pthread_create (&thread, NULL, await_request, data);

 	DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Preprocessor: ProfiNet Initialized\n"));

    /*
     * parse the argument list from the rules file
     */
    //ParseProfiNetArgs(args);

    /*
     * perform any other initialization functions that are required here
     */

    /*
     * Set the preprocessor function into the function list
     */
    AddFuncToPreprocList(sc, DetectProfiNetPackets, PRIORITY_NETWORK, PP_PROFINET, PROTO_BIT__ALL );
//    AddFuncToPreprocCleanExitList(ProfiNetCleanExit, NULL, PRIORITY_LAST, PP_ENABLE_ALL);

error:

	return;
}

/*
static bool
dissector_try_heuristic(heur_subdissector_list, uint8_t* data, Packet* p, tree, &hdtbl_entry, uint16_t u16FrameID)
{




}
*/


/*
 * dissect_pn_rt - The dissector for the Soft-Real-Time protocol
 */
static void
DissectPNRT(Packet* p, Truffle* truffle)
{
	(void) truffle;

	uint8_t* data_pointer = (uint8_t*) p->data;

	if (!p->data) {
		printf("ERROR: no data pointer\n");

		data_pointer = (uint8_t*) (p->pkt + 14);
	}


    int         pdu_len;
    int         data_len;
    uint16_t      u16FrameID;
    uint8_t       u8DataStatus;
    uint8_t       u8TransferStatus;
    uint16_t      u16CycleCounter;
    const char *pszProtAddInfo;
    const char *pszProtShort;
    const char *pszProtSummary;
    const char *pszProtComment;
   // proto_tree  *pn_rt_tree, *ti;
    char        szFieldSummary[100];
//    tvbuff_t    *next_tvb;
    bool     bCyclic;
//    heur_dtbl_entry_t *hdtbl_entry;


    /* If the link-layer dissector for the protocol above us knows whether
     * the packet, as handed to it, includes a link-layer FCS, what it
     * hands to us should not include the FCS; if that's not the case,
     * that's a bug in that dissector, and should be fixed there.
     *
     * If the link-layer dissector for the protocol above us doesn't know
     * whether the packet, as handed to us, includes a link-layer FCS,
     * there are limits as to what can be done there; the dissector
     * ultimately needs a "yes, it has an FCS" preference setting, which
     * both the Ethernet and 802.11 dissectors do.  If that's not the case
     * for a dissector, that's a deficiency in that dissector, and should
     * be fixed there.
     *
     * Therefore, we assume we are not handed a packet that includes an
     * FCS.  If we are ever handed such a packet, either the link-layer
     * dissector needs to be fixed or the link-layer dissector's preference
     * needs to be set for your capture (even if that means adding such
     * a preference).  This dissector (and other dissectors for protcols
     * running atop the link layer) should not attempt to process the
     * FCS themselves, as that will just break things. */

    /* Initialize variables */
  //  pn_rt_tree = NULL;
   // ti         = NULL;

    /*
     * Set the columns now, so that they'll be set correctly if we throw
     * an exception.  We can set them (or append things) later again ....
     */
/*
    col_set_str(pinfo->cinfo, COL_PROTOCOL, "PN-RT");
    col_set_str(pinfo->cinfo, COL_INFO, "PROFINET Real-Time");

    pdu_len = tvb_reported_length(tvb);
    if (pdu_len < 6) {
        dissect_pn_malformed(tvb, 0, pinfo, tree, pdu_len);
        return;
    }
*/
	pdu_len = p->pkth->pktlen;
    /* build some "raw" data */

//	printf("paket_len: %d\n", pdu_len);

	uint16_t* value = (uint16_t*) data_pointer;
//	printf("0x%04x\n", *value);

	u16FrameID = ntohs(*value);

//	u16FrameID = ntohs(*((uint16_t*) data_pointer));
	//data_pointer += 2;
	//data_pointer += 2;

//	printf("check 0\n");
//    u16FrameID = tvb_get_ntohs(tvb, 0);
    if (u16FrameID <= 0x001F) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0x0000-0x001F: Reserved ID";
        bCyclic         = false;
    } else if (u16FrameID <= 0x0021) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "Synchronization, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0x0020-0x0021: Real-Time: Sync (with follow up)";
        bCyclic         = false;
    } else if (u16FrameID <= 0x007F) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0x0022-0x007F: Reserved ID";
        bCyclic         = false;
    } else if (u16FrameID <= 0x0081) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "Synchronization, ";
        pszProtSummary  = "Isochronous-Real-Time";
        pszProtComment  = "0x0080-0x0081: Real-Time: Sync (without follow up)";
        bCyclic         = false;
    } else if (u16FrameID <= 0x00FF) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0x0082-0x00FF: Reserved ID";
        bCyclic         = false;
    } else if (u16FrameID <= 0x6FF) {
        pszProtShort    = "PN-RTC3";
        pszProtAddInfo  = "RTC3, ";
        pszProtSummary  = "Isochronous-Real-Time";
        pszProtComment  = "0x0100-0x06FF: RED: Real-Time(class=3): non redundant, normal or DFP";
        bCyclic         = true;
    } else if (u16FrameID <= 0x0FFF) {
        pszProtShort    = "PN-RTC3";
        pszProtAddInfo  = "RTC3, ";
        pszProtSummary  = "Isochronous-Real-Time";
        pszProtComment  = "0x0700-0x0FFF: RED: Real-Time(class=3): redundant, normal or DFP";
        bCyclic         = true;
    } else if (u16FrameID <= 0x7FFF) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0x1000-0x7FFF: Reserved ID";
        bCyclic         = false;
    } else if (u16FrameID <= 0xBBFF) {
        pszProtShort    = "PN-RTC1";
        pszProtAddInfo  = "RTC1, ";
        pszProtSummary  = "cyclic Real-Time";
        pszProtComment  = "0x8000-0xBBFF: Real-Time(class=1 unicast): non redundant, normal";
        bCyclic         = true;
    } else if (u16FrameID <= 0xBFFF) {
        pszProtShort    = "PN-RTC1";
        pszProtAddInfo  = "RTC1, ";
        pszProtSummary  = "cyclic Real-Time";
        pszProtComment  = "0xBC00-0xBFFF: Real-Time(class=1 multicast): non redundant, normal";
        bCyclic         = true;
    } else if (u16FrameID <= 0xF7FF) {
        /* check if udp frame on PNIO port */

		printf("check 1\n");

		// TODO check if p->udph->uh_dport contains the right information -- potential bug!!
		if (ntohs(p->udph->uh_dport) == 0x8892)
        { /* UDP frame */
            pszProtShort = "PN-RTCUDP,";
            pszProtAddInfo = "RT_CLASS_UDP, ";
            pszProtComment = "0xC000-0xF7FF: Real-Time(UDP unicast): Cyclic";
        }
        else
        { /* layer 2 frame */
            pszProtShort = "PN-RT";
            pszProtAddInfo = "RTC1(legacy), ";
            pszProtComment = "0xC000-0xF7FF: Real-Time(class=1 unicast): Cyclic";
        }
        pszProtSummary  = "cyclic Real-Time";
        bCyclic         = true;
    } else if (u16FrameID <= 0xFBFF) {

		printf("check 2\n");
		// TODO ehck if p->udph->uh_dport contains the right information -- portential bug!!
        if (ntohs(p->udph->uh_dport) == 0x8892)
        { /* UDP frame */
            pszProtShort = "PN-RTCUDP,";
            pszProtAddInfo = "RT_CLASS_UDP, ";
            pszProtComment = "0xF800-0xFBFF:: Real-Time(UDP multicast): Cyclic";
        }
        else
        { /* layer 2 frame */
            pszProtShort = "PN-RT";
            pszProtAddInfo = "RTC1(legacy), ";
            pszProtComment = "0xF800-0xFBFF: Real-Time(class=1 multicast): Cyclic";
         }
        pszProtSummary  = "cyclic Real-Time";
        bCyclic         = true;
    } else if (u16FrameID <= 0xFDFF) {
        pszProtShort    = "PN-RTA";
        pszProtAddInfo  = "Reserved, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFC00-0xFDFF: Reserved";
        bCyclic         = false;
        if (u16FrameID == 0xfc01) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "Alarm High, ";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: Acyclic PN-IO Alarm high priority";
        }

    } else if (u16FrameID <= 0xFEFF) {
        pszProtShort    = "PN-RTA";
        pszProtAddInfo  = "Reserved, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFE00-0xFEFF: Real-Time: Reserved";
        bCyclic         = false;
        if (u16FrameID == 0xFE01) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "Alarm Low, ";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: Acyclic PN-IO Alarm low priority";
        }
        if (u16FrameID == FRAME_ID_DCP_HELLO) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: DCP (Dynamic Configuration Protocol) hello";

        }
        if (u16FrameID == FRAME_ID_DCP_GETORSET) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: DCP (Dynamic Configuration Protocol) get/set";

        }
        if (u16FrameID == FRAME_ID_DCP_IDENT_REQ) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: DCP (Dynamic Configuration Protocol) identify multicast request";

        }
        if (u16FrameID == FRAME_ID_DCP_IDENT_RES) {
            pszProtShort    = "PN-RTA";
            pszProtAddInfo  = "";
            pszProtSummary  = "acyclic Real-Time";
            pszProtComment  = "Real-Time: DCP (Dynamic Configuration Protocol) identify response";

        }
    } else if (u16FrameID <= 0xFF01) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "RTA Sync, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFF00-0xFF01: PTCP Announce";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF1F) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "RTA Sync, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFF02-0xFF1F: Reserved";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF21) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "Follow Up, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFF20-0xFF21: PTCP Follow Up";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF22) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "Follow Up, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFF22-0xFF3F: Reserved";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF43) {
        pszProtShort    = "PN-PTCP";
        pszProtAddInfo  = "Delay, ";
        pszProtSummary  = "acyclic Real-Time";
        pszProtComment  = "0xFF40-0xFF43: Acyclic Real-Time: Delay";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF7F) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "Reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0xFF44-0xFF7F: reserved ID";
        bCyclic         = false;
    } else if (u16FrameID <= 0xFF8F) {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "";
        pszProtSummary  = "Fragmentation";
        pszProtComment  = "0xFF80-0xFF8F: Fragmentation";
        bCyclic         = false;
    } else {
        pszProtShort    = "PN-RT";
        pszProtAddInfo  = "Reserved, ";
        pszProtSummary  = "Real-Time";
        pszProtComment  = "0xFF90-0xFFFF: reserved ID";
        bCyclic         = false;
    }


    /* decode optional cyclic fields at the packet end and build the summary line */
    if (bCyclic) {
        /* cyclic transfer has cycle counter, data status and transfer status fields at the end */
       // u16CycleCounter  = tvb_get_ntohs(tvb, pdu_len - 4);
       // u8DataStatus     = tvb_get_guint8(tvb, pdu_len - 2);
       // u8TransferStatus = tvb_get_guint8(tvb, pdu_len - 1);

		// TODO check if the right values are cought
		u16CycleCounter  = ntohs(*((uint16_t*) (data_pointer + pdu_len - 4) ));
		u8DataStatus     = data_pointer[pdu_len - 2];
		u8TransferStatus = data_pointer[pdu_len - 1];

        snprintf (szFieldSummary, sizeof(szFieldSummary),
                "%sID:0x%04x, Len:%4u, Cycle:%5u (%s,%s,%s,%s)",
                pszProtAddInfo, u16FrameID, pdu_len - 2 - 4, u16CycleCounter,
                (u8DataStatus & 0x04) ? "Valid"   : "Invalid",
                (u8DataStatus & 0x01) ? "Primary" : "Backup",
                (u8DataStatus & 0x20) ? "Ok"      : "Problem",
                (u8DataStatus & 0x10) ? "Run"     : "Stop");

        /* user data length is packet len - frame id - optional cyclic status fields */
        data_len = pdu_len - 2 - 4;
    } else {
        /* satisfy the gcc compiler, so it won't throw an "uninitialized" warning */
        u16CycleCounter     = 0;
        u8DataStatus        = 0;
        u8TransferStatus    = 0;

        /* acyclic transfer has no fields at the end */
        snprintf (szFieldSummary, sizeof(szFieldSummary),
                  "%sID:0x%04x, Len:%4u",
                pszProtAddInfo, u16FrameID, pdu_len - 2);

        /* user data length is packet len - frame id field */
        data_len = pdu_len - 2;
    }

	printf("PROFINET %s, %s\n", pszProtSummary, szFieldSummary);
	printf("FrameID: 0x%04x (%s)", u16FrameID, pszProtComment);

	if (bCyclic) {
		printf("CycleCounter: %u", u16CycleCounter);

		if (u8TransferStatus) {

			printf("TransferStatus: 0x%02x (ignore this frame)", u8TransferStatus);
		} else {

			printf("TransferStatus: 0x%02x (OK)", u8TransferStatus);
		}
	}


/*
    // build protocol tree only, if tree is really used
    if (tree) {
       // build pn_rt protocol tree with summary line
        if (pn_rt_summary_in_tree) {
          ti = proto_tree_add_protocol_format(tree, proto_pn_rt, tvb, 0, pdu_len,
                "PROFINET %s, %s", pszProtSummary, szFieldSummary);
        } else {
            ti = proto_tree_add_item(tree, proto_pn_rt, tvb, 0, pdu_len, ENC_NA);
        }
        pn_rt_tree = proto_item_add_subtree(ti, ett_pn_rt);

        // add frame ID
        proto_tree_add_uint_format(pn_rt_tree, hf_pn_rt_frame_id, tvb,
          0, 2, u16FrameID, "FrameID: 0x%04x (%s)", u16FrameID, pszProtComment);

        if (bCyclic) {
            // add cycle counter
            proto_tree_add_uint_format(pn_rt_tree, hf_pn_rt_cycle_counter, tvb,
              pdu_len - 4, 2, u16CycleCounter, "CycleCounter: %u", u16CycleCounter);

            // add data status subtree
            dissect_DataStatus(tvb, pdu_len - 2, pn_rt_tree, u8DataStatus);

            // add transfer status
            if (u8TransferStatus) {
                proto_tree_add_uint_format(pn_rt_tree, hf_pn_rt_transfer_status, tvb,
                    pdu_len - 1, 1, u8TransferStatus,
                    "TransferStatus: 0x%02x (ignore this frame)", u8TransferStatus);
            } else {
                proto_tree_add_uint_format(pn_rt_tree, hf_pn_rt_transfer_status, tvb,
                    pdu_len - 1, 1, u8TransferStatus,
                    "TransferStatus: 0x%02x (OK)", u8TransferStatus);
            }
        }
    }

    // update column info now
    col_add_str(pinfo->cinfo, COL_INFO, szFieldSummary);
    col_set_str(pinfo->cinfo, COL_PROTOCOL, pszProtShort);
*/
    // get frame user data tvb (without header and footer)
//    next_tvb = tvb_new_subset_length(tvb, 2, data_len);

	data_pointer += 2;

    // ask heuristics, if some sub-dissector is interested in this packet payload
 //   if (!dissector_try_heuristic(heur_subdissector_list, next_tvb, pinfo, tree, &hdtbl_entry, GUINT_TO_POINTER( (guint32) u16FrameID))) {
        // col_set_str(pinfo->cinfo, COL_INFO, "Unknown");

        // Oh, well, we don't know this; dissect it as data.
//        dissect_pn_undecoded(next_tvb, 0, pinfo, tree, tvb_captured_length(next_tvb));
//    }

}



/*
 * Function: ParseProfiNetArgs(char *)
 *
 * Purpose: Process the preprocessor arguements from the rules file and
 *          initialize the preprocessor's data struct.  This function doesn't
 *          have to exist if it makes sense to parse the args in the init
 *          function.
 *
 * Arguments: args => argument list
 *
 * Returns: void function
 *
 */
/*
static void ParseProfiNetArgs(char *args)
{
    // your parsing function goes here, check out the other spp files
    //   for examples
}
*/

int PacketIsEtherProfi(Packet *p)
{

    if (!(p->eh))
    {
        return 0;
    }

    if (ntohs(p->eh->ether_type) != PROFI_ETHER_TYPE)
    {
        return 0;
    }

    return 1;

}

bool IsProfinet(Packet *p)
{
	// TODO make the proper testing for profnet packaets
	return PacketIsEtherProfi(p);
}

/*
 * Function: PreprocFunction(Packet *)
 *
 * Purpose: Perform the preprocessor's intended function.  This can be
 *          simple (statistics collection) or complex (IP defragmentation)
 *          as you like.  Try not to destroy the performance of the whole
 *          system by trying to do too much....
 *
 * Arguments: p => pointer to the current packet data struct
 *
 * Returns: void function
 *
 */
static void DetectProfiNetPackets(Packet *p, void *context)
{
	(void) context;
	int n;

//	tvbuff_t* tvb = tvb_new_real_data(p->pkt, p->pkth->pktlen, p->pkth->caplen);

	if (IsProfinet(p)) {



	}

	Truffle truffle;
//	tvbuff_t tb;

//	tvbuff_t* tvb = new_tvbuff_t(p->pkt, p->pkth->pktlen);


	//if (PacketIsPNRT(p)) {

	if (PacketIsEtherProfi(p)) {

//		DissectPNRT(p, &truffle);

		memcpy(&truffle.ether_src, p->eh->ether_src, 6);
		memcpy(&truffle.ether_dst, p->eh->ether_dst, 6);
				

		if (data->client_detected) {

			n = write (data->client_sockfd, (void*) &truffle, sizeof(Truffle));
			check (n >= 0, "error writing to socket");
		}
	}


//	DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "spp_profinet: type:0x%X\n", ntohs(p->eh->ehter_type)));
/*
	if (PacketIsEtherProfi(p)) {

 		DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Preprocessor: ProfiNet detected Ethertype 0x8892\n"));

		if (data->client_detected) {


 			DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Preprocessor: ProfiNet client is receiving\n"));

			n = write (data->client_sockfd, (void*) p->pkt, 100);
			check (n >= 0, "error writing to socket");
		}

	}
*/
	/*

    char *read_ptr;
    char *start = (char *) DecodeBuffer;
    char *write_ptr;
    char *end;
    int normalization_required = 0;

    if (!(p->preprocessors & PP_PROFINET_DECODE))
    {
        return;
    }

    if (!(PacketIsEtherProfi(p)))
    {
        return;
    }

    read_ptr = p->data;
    end = p->data + p->dsize;

	while (read_ptr) {

		switch (*read_ptr)
		{
		case TYPE_HD:
			// do smting
			break;
		case TYPE_YS:
			// do smting
			break;
		case TYPE_WS:
			// do smmm
			break;
		default:
			break;
		}
		read_ptr++;
	}

	*/

    /* your preproc function goes here.... */

    /*
     * if you need to issue an alert from your preprocessor, check out
     * event_wrapper.h, there are some useful helper functions there
     */
error:

	if (newsockfd >= 0) {
		close (newsockfd);
	}
	if (sockfd >= 0) {
		close (sockfd);
	}



}

/*
 * Function: PreprocCleanExitFunction(int, void *)
 *
 * Purpose: This function gets called when Snort is exiting, if there's
 *          any cleanup that needs to be performed (e.g. closing files)
 *          it should be done here.
 *
 * Arguments: signal => the code of the signal that was issued to Snort
 *            data => any arguments or data structs linked to this
 *                    functioin when it was registered, may be
 *                    needed to properly exit
 *
 * Returns: void function
 */
static void ProfiNetCleanExit(int signal, void *datas)
{
	(void) signal;
	(void) datas;

	if (data->server_sockfd >= 0) {

		close (data->server_sockfd);
	}
	if (data->client_sockfd >= 0) {

		close (data->client_sockfd);
	}

	// TODO delete socket file
       /* clean exit code goes here */
}


/*
 * Function: PreprocRestartFunction(int, void *)
 *
 * Purpose: This function gets called when Snort is restarting on a SIGHUP,
 *          if there's any initialization or cleanup that needs to happen
 *          it should be done here.
 *
 * Arguments: signal => the code of the signal that was issued to Snort
 *            data => any arguments or data structs linked to this
 *                    functioin when it was registered, may be
 *                    needed to properly exit
 *
 * Returns: void function
 */
static void PreprocRestartFunction(int signal, void *foo)
{
	(void) signal;
	(void) foo;
       /* restart code goes here */
}
#ifdef SNORT_RELOAD

static void ProfiNetReload(struct _SnortConfig *sc, char *args, void **new_config) {



}

static void * ProfiNetReloadSwap(struct _SnortConfig *sc, void *swap_config) {


}

static void ProfiNetReloadSwapFree(void *data) {

}
#endif
