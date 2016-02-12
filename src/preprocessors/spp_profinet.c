/**
 * @file spp_profinet.c
 *
 * $Id$
 * @brief Snort Preprocessor Plugin Source File ProfiNet
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

/** Snort includes **/



/** Profinet includes **/
#include "Profinet/DissectorRegister.h"
#include "Profinet/DissectorRegister-int.h"

#include "Profinet/Dissector.h"
#include "Profinet/Dissector-int.h"
#include "Profinet/PNRTDissector.h"

#include "Profinet/Sender.h"
#include "Profinet/Sender-int.h"
#include "Profinet/UnixSocketSender.h"



 /** The top level dissector register. **/
 DissectorRegister_t *tlRegister;
 /** The ipc sender. **/
 Sender_t *sender;

static void ProfiNetInit(struct _SnortConfig *, char *args);
static void DetectProfiNetPackets(Packet *, void *context);
static void ProfiNetCleanExit(int, void *);

/**
 * Registers the preprocessor keyword and initialization
 * function into the preprocessor list.  This is the function that
 * gets called from InitPreprocessors() in plugbase.c.
 *
 */
void SetupProfiNet()
{

}

/**
 * Initializes the dissectors for the profinet protocols.
 *
 */
void DissectorInit()
{
  tlRegister = DissectorRegister_new();
  Dissector_t *pnrtDissector = PNRTDissector_new();

  tlRegister->ops->DissectorRegister_insert(tlRegister, prntDissector);

}

/**
 * Calls the argument parsing function, performs final setup on data
 * structs, links the preproc function into the function list.
 *
 * @param sc the snort configuration
 * @param args ptr to argument string
 *
 */
static void ProfiNetInit(struct _SnortConfig * sc, char *args)
{

  sender = UnixSocketSender_new();

  DissectorInit();
}

/**
 *  Perform the preprocessor's intended function.  This can be
 *  simple (statistics collection) or complex (IP defragmentation)
 *  as you like.  Try not to destroy the performance of the whole
 *  system by trying to do too much....
 *
 * @param p pointer to the current packet data struct
 * @param context the settings contex of the preprocessor
 */
static void DetectProfiNetPackets(Packet *p, void *context)
{
  Dissector_t *topDissector = topLR->ops->get16(p->eh->ether_type);

  Buffer_t *buffer = Buffer_new(p);
  ProtocolTree_t *tree = ProtocolTree_new();

  topDissector->ops->Dissector_dissect(topDissector, buffer, tree);

  Buffer_free(buffer);

  Truffle_t truffle = Truffle_new(tree);

  ProtocolTree_free(tree);

  sender->ops->Sender_send(sender, truffle);
}

/**
 * This function gets called when Snort is exiting, if there's
 * any cleanup that needs to be performed (e.g. closing files)
 * it should be done here.
 *
 * @param signal the code of the signal that was issued to Snort
 *
 * @param data any arguments or data structs linked to this
 *             functioin when it was registered, may be
 *             needed to properly exit
 */
static void ProfiNetCleanExit(int signal, void *datas)
{

}

#endif
