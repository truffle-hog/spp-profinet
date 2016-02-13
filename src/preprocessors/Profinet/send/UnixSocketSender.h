#ifndef __UNIX_SOCKET_SENDER_H__
#define __UNIX_SOCKET_SENDER_H__

#include "send/Sender.h"
#include "send/Truffle.h"

/**
 * @see Sender_new
 */
Sender_t *UnixSocketSender_new();

/**
 * @see Sender_free
 */
int UnixSocketSender_free(Sender_t *sender);

/**
 * @see Sender_send
 */
int UnixSocketSender_send(Sender_t *this, Truffle_t *truffle);

#endif
