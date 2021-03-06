#pragma once

#ifdef __cplusplus
extern "C" {
#endif

struct FTPServer;

#include "server.h"
#include "compat/compat.h"
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
	char rnfr[PATH_MAX];
	unsigned short actv;
	bool authorized;
	bool write_enabled;

	char mode;
	char stru;
	char type;
};

struct FTPClient* ftpclient_create(int, struct FTPServer*, char*, int);

void ftpclient_send_message(struct FTPClient*, int, bool, const char*);

bool ftpclient_data_start(struct FTPClient*, void (*)(struct FTPClient*), bool);
void ftpclient_data_end(struct FTPClient*);
bool ftpclient_data_pasv(struct FTPClient*);

void ftpclient_event(struct FTPClient*, int);

void ftpclient_disconnect(struct FTPClient*, int);
void ftpclient_destroy(struct FTPClient*);

void string_getmode(char mode[11], struct stat* st);
void string_getpath(char*, const char*, const char*);

#ifdef __cplusplus
}
#endif
