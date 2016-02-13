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

#include "send/Sender-int.h"
#include "send/Sender.h"
#include "send/UnixSocketSender.h"

#include "dissect/Dissector.h"
#include "dissect/Dissector-int.h"
#include "dissect/dissectors/PacketDissector.h"

#include "dissect/tree/ProtocolTree.h"
#include "dissect/tree/ProtocolTree-int.h"

#include "dissect/buffer/Buffy.h"
#include "dissect/buffer/Buffy-int.h"

#include "send/Truffle.h"

/*
 * put in other inculdes as necessary
 */

/*
 * your preprocessor header file goes here if necessary, don't forget
 * to include the header file in plugbase.h too!
 */
#include "spp_profinet.h"


/*
 * If you need to instantiate the preprocessor's
 * data structure, do it here
 */

Sender_t *sender;
Dissector_t *packetDissector;

long long n = 0;


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

	packetDissector = PacketDissector_new();
	check_mem(packetDissector);

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
void printLabel(struct Value val) {


	switch(val.type) {

		case is_string:
			printf("%s", val.val.string);
			break;

		case is_char:
			printf("%c", val.val.character);
			break;

		case is_int8:
			printf("%d", val.val.int8);
			break;
		case is_int16:
			printf("%d", val.val.int16);
			break;
		case is_int32:
			printf("%d", val.val.int32);
			break;
		case is_int64:
			printf("%ld", val.val.int64);
			break;

		case is_uint8:
			printf("%02X", val.val.uint8);
			break;
		case is_uint16:
			printf("%04X", val.val.uint16);
			break;
		case is_uint32:
			printf("%08X", val.val.uint32);
			break;
		case is_uint64:
			printf("%016lX", val.val.uint64);
			break;

		case is_float:
			printf("%f", val.val.float32);
			break;
		case is_double:
			printf("%f", val.val.double64);
			break;

	}
	return;
}

void ProtocolTree_printDot(struct ProtocolNode *tree) {

//	char dot[tree->treeData->size * 20];

    printf("digraph protocol\n{\n");
    printf("node [shape=record];\n");

    char *current;

	int i = 0;
	for (i = 0; i < tree->treeData->size; i++) {

        current = tree->treeData->mappedNodePointers[i]->key;

        printf("\t%s [label=\"{%s|", current, current);
        printLabel(tree->treeData->mappedNodePointers[i]->value);
        printf("}\"];\n");

        int j = 0;
		for (j = 0; j < tree->treeData->mappedNodePointers[i]->childCount; j++ ) {

            printf("\t%s -> %s;\n", current, tree->treeData->mappedNodePointers[i]->children[j]->key);
		}
	}
    printf("}\n");
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

	ProtocolTree_t *protoTree = ProtocolTree_new("frame");
    check_mem(protoTree);

	Buffy_t *buffy = Buffy_new(p);
    check_mem(buffy);

	packetDissector->ops->Dissector_dissect(packetDissector, buffy, protoTree);

    //TODO implement to check if the protoTree built a Profinet package or not here if not just discard everything

	Truffle_t *truffle = Truffle_new(protoTree);
    check_mem(truffle);

	if (truffle) {
		sender->ops->Sender_send(sender, truffle);
	}
    ProtocolTree_printDot(protoTree);
	//protoTree->ops->ProtocolTree_toString(protoTree);

 //   protoTree->ops->ProtocolTree_free(protoTree);
    //buffy->ops->Buffy_freeChain(buffy); //TODO implement this function in Buffy
  //  free(truffle);

error:
    return;

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
   // sender->ops->Sender_free(sender);
   // packetDissector->ops->Dissector_free(packetDissector);
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
