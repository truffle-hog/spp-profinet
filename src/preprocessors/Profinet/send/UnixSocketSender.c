/**
 * @file
 * @brief This file houses the operations that are specific for a UnixSocketSender
 *
 * UnixSocketSender uses Unix sockets for sending a Truffle to a listening client.
 */



#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <pwd.h>
#include <pthread.h> // threads

#include "dbg.h"

#include "send/Sender.h"
#include "send/Sender-int.h"
#include "send/UnixSocketSender.h"

static const int TRUFFLEHOG_CONNECT_REQUEST = 0x0;
static const int TRUFFLEHOG_DISCONNECT_REQUEST = 0x1;
static const int SNORT_CONNECT_RESPONSE = 0x2;
static const int SNORT_DISCONNECT_RESPONSE = 0x3;

typedef struct SocketData {
	int server_sockfd;
	int client_sockfd;

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

int UnixSocketSender_free(Sender_t *sender);
int UnixSocketSender_send(Sender_t *this, Truffle_t *truffle);

/**
 * @see Sender_ops
 */
static const struct Sender_ops UnixSocketSenderOverride_ops = {
  UnixSocketSender_free,
  UnixSocketSender_send
};

/**
 * @brief returns the socket file name (with correct home path)
 * Converts the given socketname to the correct path name in the homedir
 *
 *
 * @param socket_name_ the name of the socket to be created
 * @return the correct name (with path) of the socket
 */
char * convert_to_homepath_name(char *socket_name_) {

	char * socket_name = socket_name_;
	char * homedir;

	if ((homedir = getenv("HOME")) == NULL) {

		homedir = getpwuid(getuid())->pw_dir;
	}
	char *socket_file = malloc(strlen(homedir) + strlen(socket_name) + 1);
	check_mem(socket_file);

	socket_file[0] = '\0';
	strcat(socket_file, homedir);
	strcat(socket_file, "/");
	strcat(socket_file, socket_name);

	printf("created socket file: %s\n", socket_file);

	return socket_file;

error:
	return NULL;
}

void * await_request( void* args) {

	(void) args;
	SocketData_t *data = (SocketData_t*) args;
	int n;
	int buffer = -1;

	data->client_sockfd = accept (data->server_sockfd, (struct sockaddr *) &(data->cli_addr), &(data->clilen));
	check(data->client_sockfd >= 0, "error on accept");

//	memset(buffer, 0, 256);



	// TODO implement a valid chancel condition
	while(true) {

		n = read(data->client_sockfd ,&buffer, sizeof(int));
		check(n >= 0, "error reading from socket");
//		int response = -1;

//		switch(buffer) {

		if (buffer == TRUFFLEHOG_CONNECT_REQUEST) {
			
			data->client_detected = true;
			debug("TruffleHog wants to connect...");
//			response = SNORT_CONNECT_RESPONSE;	
			
		} else if (buffer == TRUFFLEHOG_DISCONNECT_REQUEST) {
			
			data->client_detected = false;
			debug("TruffleHog wants to disconnect...");
//			response = SNORT_DISCONNECT_RESPONSE;
			
		} else {
			
			sentinel("there is no allowed default case yet");
		}
//		}
//		n = write(data->client_sockfd, (void*) &response, sizeof(int));
//		check (n >= 0, "error writing to socket");

	//	DEBUG_WRAP(DebugMessage(DEBUG_PLUGIN, "Received message: %s\n",buffer));

		// TODO instead of toggling request a specific ID from the client
		// something secure

	//	data->client_detected = !data->client_detected;;
	}

	return NULL;
error:
	// TODO error handling
	return NULL;

}

/**
 * @see Sender_new
 */
Sender_t *
UnixSocketSender_new() {

	struct UnixSocketSender *unixSocketSender = malloc(sizeof(struct UnixSocketSender));
  	check_mem(unixSocketSender);

	unixSocketSender->socketData.client_detected = false;

	// TODO check if there will be a memory leak if I dereference the pointer for the value
	unixSocketSender->sender = *Sender_new(&UnixSocketSenderOverride_ops);
	check_mem(&unixSocketSender->sender);
	// declare some variabled in use
	pthread_t thread;
	int bind_check, len;

	// open the unix socket, as sock_seqpacket
	unixSocketSender->socketData.server_sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	check(unixSocketSender->socketData.server_sockfd != -1, "cannot open socket");

	memset((char *) &(unixSocketSender->socketData.serv_addr), 0, sizeof(unixSocketSender->socketData.serv_addr));
	char * socket_file = convert_to_homepath_name("socket.sock");
	check_mem(socket_file);

	unixSocketSender->socketData.serv_addr.sun_family = AF_UNIX;
	strcpy(unixSocketSender->socketData.serv_addr.sun_path, socket_file);
	len = strlen(unixSocketSender->socketData.serv_addr.sun_path) + sizeof(unixSocketSender->socketData.serv_addr.sun_family);

	unlink(socket_file);
	bind_check = bind(unixSocketSender->socketData.server_sockfd, (struct sockaddr *) &(unixSocketSender->socketData.serv_addr), len);
	check(bind_check != -1, "error on binding");

	// TODO check again what the 5 means and if there is anything that has to be declared differently
	listen(unixSocketSender->socketData.server_sockfd, 5);
	unixSocketSender->socketData.clilen = sizeof(unixSocketSender->socketData.cli_addr);
	pthread_create (&thread, NULL, await_request, &unixSocketSender->socketData);


	return (Sender_t*) unixSocketSender;


error:
  	return NULL;
}

/**
 * @see Sender_free
 */
int UnixSocketSender_free(Sender_t *sender) {

	struct UnixSocketSender* unixSocketSender = (struct UnixSocketSender*) sender;

	Sender_free(&unixSocketSender->sender);

	free(unixSocketSender);

	return 0;

	// TODO is there  any error handling to be done?
}

/**
 * @see Sender_send
 */
int UnixSocketSender_send(Sender_t *this, Truffle_t *truffle) {

	int n = 0;
	struct UnixSocketSender* unixSocketSender = (struct UnixSocketSender*) this;

	if (unixSocketSender->socketData.client_detected) {

		n = write(unixSocketSender->socketData.client_sockfd, (void*) truffle, sizeof(Truffle_t));

		check (n >= 0, "error writing to socket");

		return n;
	}

  return 0;

error:
	return -1;

}
