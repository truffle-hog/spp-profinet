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

 /** The top level dissector register. **/
 DissectorRegister_t *tlRegister;
 /** The ipc sender. **/
 Sender_t *sender;

static void ProfiNetInit(struct _SnortConfig *, char *args);
static void DetectProfiNetPackets(Packet *, void *context);
static void ProfiNetCleanExit(int, void *);

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

}

void DissectorInit()
{
  tlRegister = DissectorRegister_new();
  Dissector_t *pnrtDissector = PNRTDissector_new();
  Interval_t pnrtInterval;
  pnrtInterval.lower = 0x8892;
  pnrtInterval.upper = 0x8892;

  tlRegister->ops->DissectorRegister_insert(tlRegister, etherInterval, prntDissector);




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

  Sender_t *sender = UnixSocketSender_new();

  DissectorInit();
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
  Dissector_t *topDissector = topLR->ops->get16(p->eh->ether_type);

  Buffer_t *buffer = Buffer_new(p);
  ProtocolTree_t *tree = ProtocolTree_new();

  topDissector->ops->Dissector_dissect(topDissector, buffer, tree);

  Buffer_free(buffer);

  Truffle_t truffle = Truffle_new(tree);

  ProtocolTree_free(tree);



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

}

#endif
