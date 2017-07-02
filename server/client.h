#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#ifdef __PSL1GHT__
#include <net/poll.h>
#else
#include <sys/poll.h>
#endif

#include <netinet/in.h>

#ifdef __CELLOS_LV2__
#include <sys/fs_external.h>
#define PATH_MAX (CELL_FS_MAX_MP_LENGTH + CELL_FS_MAX_FS_PATH_LENGTH + 1)
#endif

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 1056
#endif

struct FTPServer;

#include "server.h"
#include "compat/fs.h"

struct FTPClient;

struct FTPClient
{
	struct FTPServer* server;

	int sock_control;
	int sock_data;
	int sock_pasv;

	struct sockaddr* addr;
	socklen_t addrlen;

	char* buf;
	int bufsiz;

	void (*data_callback)(struct FTPClient*);

	// ftp variables
	struct FTPFileHandle* handle;

	char cwd[PATH_MAX];
	char username[32];
	off_t rest;
	char* rnfr;
	unsigned short actv;
	bool authorized;
};

struct FTPClient* ftpclient_create(int, struct FTPServer*, char*, int);

void ftpclient_send_message(struct FTPClient*, int, bool, const char*);

bool ftpclient_data_start(struct FTPClient*, void (*)(struct FTPClient*), bool);
void ftpclient_data_end(struct FTPClient*);
bool ftpclient_data_pasv(struct FTPClient*);

void ftpclient_event(struct FTPClient*, int);

void ftpclient_disconnect(struct FTPClient*, int);
void ftpclient_destroy(struct FTPClient*, bool);

#ifdef __cplusplus
}
#endif
