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
#include <signal.h>
#include <sys/stat.h>

#include <pwd.h>
#include <pthread.h> // threads

#include "dbg.h"

#include "send/Sender.h"
#include "send/Sender-int.h"
#include "send/UnixSocketSender.h"

static const int TRUFFLEHOG_CONNECT_REQUEST = 0x0;
static const int TRUFFLEHOG_DISCONNECT_REQUEST = 0x1;
static const int SNORT_CONNECT_RESPONSE = 0x2;



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
	char *socket_file = malloc(strlen(homedir) + strlen(socket_name) + 2);
	check_mem(socket_file);

	socket_file[0] = '\0';
	strcat(socket_file, homedir);
	strcat(socket_file, "/");
	strcat(socket_file, socket_name);

	debug("created socket file: %s", socket_file);

	return socket_file;

error:
	return NULL;
}

void * await_request( void* args) {

	SocketData_t *data = (SocketData_t*) args;
	int n;
	int buffer = -1;

	data->client_sockfd = accept (data->server_sockfd, (struct sockaddr *) &(data->cli_addr), &(data->clilen));
	check(data->client_sockfd >= 0, "error on accept");

  //check(unlink(data->serv_addr.sun_path) >= 0, "error on unlinking: %s", data->serv_addr.sun_path);

//	memset(buffer, 0, 256);

	bool isConnected = true;

	// TODO implement a valid chancel condition
	while(isConnected) {

		n = read(data->client_sockfd ,&buffer, sizeof(int));
		check(n >= 0, "error reading from socket");
		int response = -1;

		if (buffer == TRUFFLEHOG_CONNECT_REQUEST) {

			data->client_detected = true;
			debug("TruffleHog wants to connect...");
			response = SNORT_CONNECT_RESPONSE;
			n = write(data->client_sockfd, (void*) &response, sizeof(int));
			check (n >= 0, "error writing to socket");

		} else if (buffer == TRUFFLEHOG_DISCONNECT_REQUEST) {

			data->client_detected = false;
			debug("TruffleHog wants to disconnect...");
			isConnected = false;
			//int retval = 0;
			//pthread_exit(&retval);

		} else {

			sentinel("there is no allowed default case yet");
		}
	}

	return await_request((void*) data);

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

	struct UnixSocketSender *unixSocketSender = (struct UnixSocketSender *) Sender_new(&UnixSocketSenderOverride_ops);
	check_mem(unixSocketSender);
	unixSocketSender = realloc(unixSocketSender, sizeof(struct UnixSocketSender));
	check_mem(unixSocketSender);

	check(signal(SIGPIPE, SIG_IGN) != SIG_ERR, "error setting pipe signal to ignore");

	unixSocketSender->socketData.client_detected = false;
	// declare some variabled in use
	//pthread_t thread;
	int bind_check, len;

	// open the unix socket, as sock_seqpacket
	unixSocketSender->socketData.server_sockfd = socket(AF_UNIX, SOCK_SEQPACKET, 0);
	check(unixSocketSender->socketData.server_sockfd != -1, "cannot open socket");

	memset((char *) &(unixSocketSender->socketData.serv_addr), 0, sizeof(unixSocketSender->socketData.serv_addr));
	char *socket_file = convert_to_homepath_name("socket.sock");
	check_mem(socket_file);

	unixSocketSender->socketData.serv_addr.sun_family = AF_UNIX;
	strcpy(unixSocketSender->socketData.serv_addr.sun_path, socket_file);
	len = strlen(unixSocketSender->socketData.serv_addr.sun_path) + sizeof(unixSocketSender->socketData.serv_addr.sun_family);

	unlink(socket_file);
	bind_check = bind(unixSocketSender->socketData.server_sockfd, (struct sockaddr *) &(unixSocketSender->socketData.serv_addr), len);
	check(bind_check != -1, "error on binding");
  free(socket_file);
  //check(
  //  chmod(socket_file, S_IROTH | S_IWOTH | S_IXOTH) >= 0, "error on setting protection on %s", socket_file);

  struct stat socketStat;
  stat(socket_file, &socketStat);
// check(
//   chmod(unixSocketSender->socketData.server_sockfd, socketStat.st_mode | S_IWOTH | S_IWGRP)  >= 0, "error on setting protection on %s", socket_file);

check(
  chmod(unixSocketSender->socketData.serv_addr.sun_path, 00777  & ~S_ISVTX)  >= 0, "error on setting protection on %s", unixSocketSender->socketData.serv_addr.sun_path);

    // check(
    //   chmod(socket_file, 0777) >= 0, "error on setting protection on %s", socket_file);



	// TODO check again what the 5 means and if there is anything that has to be declared differently
	listen(unixSocketSender->socketData.server_sockfd, 5);
	unixSocketSender->socketData.clilen = sizeof(unixSocketSender->socketData.cli_addr);

	check(
		pthread_create (&unixSocketSender->socketData.thread, NULL, await_request, &unixSocketSender->socketData) == 0, "error creating pthread");

	return (Sender_t*) unixSocketSender;


error:
  	return NULL;
}

/**
 * @see Sender_free
 */
int UnixSocketSender_free(Sender_t *sender) {

  debug("Freeing the unix socket sender");

	struct UnixSocketSender* unixSocketSender = (struct UnixSocketSender*) sender;

	struct stat status;

	stat(unixSocketSender->socketData.serv_addr.sun_path, &status);


	debug("executing user id: %08o", getuid());
	debug("user id of socket file owner: %08o", status.st_uid);

	debug("executing group id: %08o", getgid());
	debug("group id of socket file owner: %08o", status.st_gid);

  debug("socket file protection: %08o", status.st_mode);

  //
	// check(
	// 	unlink(unixSocketSender->socketData.serv_addr.sun_path) >= 0, "error on unlinking: %s", unixSocketSender->socketData.serv_addr.sun_path);

	// check(
	// 	pthread_kill(unixSocketSender->socketData.thread, SIGKILL) >= 0, "error sending signal to pthread");

  debug("shutting down server sockfd");
	check(
		shutdown(unixSocketSender->socketData.server_sockfd, SHUT_RDWR) >= 0, "error on shuting down server socket");


    check(
      pthread_kill(unixSocketSender->socketData.thread, SIGKILL) >= 0, "error sending signal to pthread");

    if(unixSocketSender->socketData.client_detected) {

      debug("shutting down client sockfd");
	check(
		shutdown(unixSocketSender->socketData.client_sockfd, SHUT_RDWR) >= 0, "error on shutting down client socket");
  }
  debug("closing server socket");
	check(
		close(unixSocketSender->socketData.server_sockfd) >= 0, "error closing server socket");


    if(unixSocketSender->socketData.client_detected) {

      debug("closing client socket");
  check(
		close(unixSocketSender->socketData.client_sockfd) >= 0, "error closing client socket");
  }

  //check(
  //  unlink(unixSocketSender->socketData.serv_addr.sun_path) >= 0, "error on unlinking: %s", unixSocketSender->socketData.serv_addr.sun_path);



	free(unixSocketSender);



	return 0;

error:
	return -1;

	// TODO is there  any error handling to be done?
}

static int
_restartSocket(Sender_t *this) {

	struct UnixSocketSender* unixSocketSender = (struct UnixSocketSender*) this;



	// TODO change to signalhandler signal
	check(
		pthread_kill(unixSocketSender->socketData.thread, SIGKILL) >= 0, "error sending signal to pthread");

	check(
		shutdown(unixSocketSender->socketData.server_sockfd, SHUT_RDWR) >= 0, "error on shuting down server socket");

	check(
		shutdown(unixSocketSender->socketData.client_sockfd, SHUT_RDWR) >= 0, "error on shutting down client socket");

	check(
		close(unixSocketSender->socketData.server_sockfd) >= 0, "error closing server socket");

	check(
		close(unixSocketSender->socketData.client_sockfd) >= 0, "error closing client socket");


	unixSocketSender->socketData.client_detected = false;
	// declare some variabled in use
	//pthread_t thread;
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

	check(
		pthread_create (&unixSocketSender->socketData.thread, NULL, await_request, &unixSocketSender->socketData) == 0, "error creating pthread");

	return 0;

error:
	return -1;

}

/**
 * @see Sender_send
 */
int UnixSocketSender_send(Sender_t *this, Truffle_t *truffle) {

	check(this != NULL, "caller must not be null");
	check(truffle != NULL, "truffle must not be null");

	int n = 0;
	struct UnixSocketSender* unixSocketSender = (struct UnixSocketSender*) this;

	if (unixSocketSender->socketData.client_detected) {

		n = write(unixSocketSender->socketData.client_sockfd, (void*) truffle, sizeof(Truffle_t));

		check(n == sizeof(Truffle_t), "error could not send the correct number of bytes");

		check_to (n >= 0, socket_error, "error writing to socket");

		return n;
	}

  return 0;

socket_error:

	debug("restarting socket connection, due to socket error");
	//pthread_exit(&unixSocketSender->socketData.thread);
	_restartSocket(this);
	//UnixSocketSender_send(this, truffle);
	return -1;

error:
	return -1;
}
