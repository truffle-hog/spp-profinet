#ifndef __UNIX_SOCKET_SENDER_H__
#define __UNIX_SOCKET_SENDER_H__

#include "send/Sender.h"
#include "send/Truffle.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

typedef struct SocketData {
	int server_sockfd;
	int client_sockfd;

	pthread_t thread;

	bool client_detected;

	struct sockaddr_un serv_addr;
	struct sockaddr_un cli_addr;
	socklen_t clilen;
} SocketData_t;

/**
 * @brief Sends Truffles to a unix socket a client is reading from.
 *
 */
struct UnixSocketSender {
   /** The encapsulated sender type for save casting. */
	struct Sender sender;
	// TODO implement specific fields / functionality
	SocketData_t socketData;
};

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
