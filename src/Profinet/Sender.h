#ifndef __SENDER_H__
#define __SENDER_H__

/**
 * @file
 * @brief The sender interface.
 *
 * The basic Sender abstraction. Every implementation of a Sender will use
 * and implement the operations described in this interface.
 * A Sender is used to send truffles to a certain port, socket, or messagequeue,
 * depending on the implementation.
 *
 */
#include "Truffle.h"

struct Sender;
typedef struct Sender Sender_t;


/**
 * @brief Frees the given sender.
 *
 * @param sender the sender to be freed
 *
 * @return 0 if the freeing was successful,
 *         -1 otherwise
 */
int Sender_free(Sender_t *sender);

/**
 * Sends the given truffle to the specified ipc
 *
 * @param this the calling sender
 * @param truffle the truffle to be send
 *
 * @return 0 if the sending was successful, -1 if no client is detected for receiving, or on other errors.
 */
int Sender_send(Sender_t *this, Truffle_t *truffle);

#endif
