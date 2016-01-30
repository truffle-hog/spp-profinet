/** @file Sender.h
* The basic Sender abstraction. Every implementation of a Sender will use
* and implement the operations described in this interface.
* A Sender is used to send truffles to a certain port, socket, or messagequeue,
* depending on the implementation.
*
*/
struct Sender;
typedef struct Sender Sender_t;


/**
* Creates a new Dissector with the given operations. This Function is the
* interface constructor for every Dissector implementation.
*
* @param ops the pointer to the operations used for this dissector
* @return a pointer to the created dissector
*/
Sender_t * Sender_new(const struct sender_ops *ops);


/**
* Frees the given dissector.
*/
void Sender_free(Sender_t *dissector);

/**
* Sends the given truffle to the specified ipc
*
* @param this the calling sender
* @param truffle the truffle to be send
*
* @return 0 if the sending was successful, -1 if no client is detected for receiving, or on other errors.
*/
int Sender_send(Sender_t *this, Truffle_t *truffle);
