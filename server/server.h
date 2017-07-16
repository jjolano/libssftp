#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct FTPClient;
struct FTPCommand;

#include "client.h"
#include "cmd.h"
#include "compat/compat.h"
#include "util/avl.h"

typedef void (*ftpclient_event_connect)(struct FTPClient*);
typedef void (*ftpclient_event_disconnect)(struct FTPClient*);

struct FTPServer
{
	bool run;
	bool stop;

	int sock;
	unsigned short port;

	struct sockaddr_in sin_addr;
	struct sockaddr* addr;

	char* buf;
	int bufsiz;

	struct pollfd* fds;
	nfds_t nfds;

	struct AVLTree* clients;
	struct FTPCommand* commands;

	ftpclient_event_connect* connect_callbacks;
	int nconnect_callbacks;

	ftpclient_event_disconnect* disconnect_callbacks;
	int ndisconnect_callbacks;
};

int ftpserv_create(struct FTPServer*, unsigned short, struct FTPCommand*);
int ftpserv_run(struct FTPServer*);
void ftpserv_stop(struct FTPServer*);
void ftpserv_destroy(struct FTPServer*);

void ftpserv_event_connect_register(struct FTPServer*, ftpclient_event_connect);
void ftpserv_event_connect_unregister(struct FTPServer*, ftpclient_event_connect);
void ftpserv_event_connect_call(struct FTPServer*, struct FTPClient*);

void ftpserv_event_disconnect_register(struct FTPServer*, ftpclient_event_disconnect);
void ftpserv_event_disconnect_unregister(struct FTPServer*, ftpclient_event_disconnect);
void ftpserv_event_disconnect_call(struct FTPServer*, struct FTPClient*);

#ifdef __cplusplus
}
#endif
