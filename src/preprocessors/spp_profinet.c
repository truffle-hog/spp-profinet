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

#include "Sender-int.h"
#include "Sender.h"
#include "UnixSocketSender.h"

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
 * If you need to instantiate the preprocessor's
 * data structure, do it here
 */

Sender_t *sender;

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

	sender = UnixSocketSender_new();

	check_mem(sender);

 	DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Preprocessor: ProfiNet Initialized\n"));

    AddFuncToPreprocList(sc, DetectProfiNetPackets, PRIORITY_NETWORK, PP_PROFINET, PROTO_BIT__ALL );
//    AddFuncToPreprocCleanExitList(ProfiNetCleanExit, NULL, PRIORITY_LAST, PP_ENABLE_ALL);

error:

	return;
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

	Truffle_t truffle;

	if (PacketIsEtherProfi(p)) {

		memcpy(&truffle.eh.sourceMacAddress, p->eh->ether_src, 6);
		memcpy(&truffle.eh.destMacAddress, p->eh->ether_dst, 6);

		sender->ops->Sender_send(sender, &truffle);					
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
