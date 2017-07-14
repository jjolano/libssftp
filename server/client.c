#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

#ifdef __PSL1GHT__
#include <net/poll.h>
#include "compat/psl1ght.h"
#else
#include <sys/poll.h>
#include <netinet/tcp.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "client.h"
#include "response.h"
#include "compat/fs.h"
#include "util/array.h"
#include "util/avl.h"

define_array(struct pollfd, pollfd_);

#ifdef __CELLOS_LV2__
#include "compat/cellos_lv2.h"
#endif

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh_exports.h"
#endif

struct FTPClient* ftpclient_create(int sock, struct FTPServer* server, char* buf, int bufsiz)
{
	struct FTPClient* client = malloc(sizeof(struct FTPClient));

	if(client != NULL)
	{
		client->server = server;

		client->sock_control = sock;
		client->sock_data = -1;
		client->sock_pasv = -1;

		client->addr = malloc(sizeof(struct sockaddr));
		client->addrlen = sizeof(struct sockaddr);
		getsockname(sock, client->addr, &client->addrlen);

		client->buf = buf;
		client->bufsiz = bufsiz;

		client->data_callback = NULL;

		client->handle = NULL;

		strcpy(client->cwd, "/");
		client->username[0] = '\0';
		client->rest = 0;
		client->rnfr[0] = '\0';
		client->actv = 20;
		client->authorized = false;
		client->write_enabled = true;

		client->mode = 'S'; // S = stream, B = block, C = compressed
		client->stru = 'F'; // F = file, R = record, P = page
		client->type = 'I'; // I = binary, A = ASCII, E = EBCDIC, L <byte size> = custom

		ftpserv_event_connect_call(server, client);
	}
	
	return client;
}

void ftpclient_send_message(struct FTPClient* client, int code, bool multi, const char* message)
{
	int len = 0;

	if(code > 0)
	{
		len = sprintf(client->buf, "%d%c%s\r\n", code, multi ? '-' : ' ', message);
	}
	else
	{
		if(multi)
		{
			len = sprintf(client->buf, " %s\r\n", message);
		}
		else
		{
			len = sprintf(client->buf, "%s\r\n", message);
		}
	}

	if(len > 0)
	{
		if(send(client->sock_control, client->buf, len, 0) <= 0)
		{
			ftpclient_disconnect(client, client->sock_control);
		}
	}
}

bool ftpclient_data_start(struct FTPClient* client, void (*callback)(struct FTPClient* client), bool outbound)
{
	if(client->sock_data == -1)
	{
		if(client->sock_pasv == -1)
		{
			// no pasv listener, attempt actv mode
			// only connect to originating point
			struct sockaddr_in* sin_addr = (struct sockaddr_in*) client->addr;
			struct sockaddr_in actv_addr;

			memcpy(&actv_addr, sin_addr, client->addrlen);

			actv_addr.sin_port = htons(client->actv);

			if(client->actv != 20)
			{
				// reset actv port if changed from default
				client->actv = 20;
			}

			int sock = socket(AF_INET, SOCK_STREAM, 0);

			if(sock == -1)
			{
				return false;
			}

			if(connect(sock, (struct sockaddr*) &actv_addr, client->addrlen) == -1)
			{
				close(sock);
				return false;
			}

			client->sock_data = sock;
		}
		else
		{
			// use pasv listener
			struct pollfd fd;
			fd.fd = client->sock_pasv;
			fd.events = POLLIN;

			int p = poll(&fd, 1, 5000);

			if(p == 1)
			{
				client->sock_data = accept(client->sock_pasv, NULL, NULL);
			}

			ftpclient_disconnect(client, client->sock_pasv);
			client->sock_pasv = -1;

			if(p <= 0 || client->sock_data == -1)
			{
				return false;
			}
		}
	}

	int optval = 1;
	setsockopt(client->sock_data, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
	setsockopt(client->sock_data, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	setsockopt(client->sock_data, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	short events = POLLIN;

	if(outbound)
	{
		events |= POLLOUT;
	}

	// enable event polling on data connection
	struct pollfd* dpfd = pollfd_array_add(&client->server->fds, client->server->nfds);

	if(dpfd != NULL)
	{
		dpfd->fd = client->sock_data;
		dpfd->events = events;

		++client->server->nfds;

		// insert data connection to reference this client
		avltree_insert(client->server->clients, client->sock_data, client);
	}
	else
	{
		// out of memory
		close(client->sock_data);
		return false;
	}

	client->data_callback = callback;
	return true;
}

void ftpclient_data_end(struct FTPClient* client)
{
	if(client->handle != NULL)
	{
		if(client->handle->_dir)
		{
			ssftpFsClosedir(client->handle);
		}
		else
		{
			ssftpFsClose(client->handle);
		}

		client->handle = NULL;
	}

	if(client->sock_data != -1)
	{
		ftpclient_disconnect(client, client->sock_data);
		client->sock_data = -1;
	}

	if(client->sock_pasv != -1)
	{
		ftpclient_disconnect(client, client->sock_pasv);
		client->sock_pasv = -1;
	}

	client->data_callback = NULL;
}

bool ftpclient_data_pasv(struct FTPClient* client)
{
	if(client->sock_data != -1)
	{
		return false;
	}

	if(client->sock_pasv != -1)
	{
		ftpclient_disconnect(client, client->sock_pasv);
		client->sock_pasv = -1;
	}

	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if(sock == -1)
	{
		return false;
	}

	int optval = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	struct sockaddr_in sin_addr;
	socklen_t len = sizeof(sin_addr);
	getsockname(client->sock_control, (struct sockaddr*) &sin_addr, &len);

	// let kernel auto-select port
	sin_addr.sin_port = 0;

	if(bind(sock, (struct sockaddr*) &sin_addr, len) == -1)
	{
		close(sock);
		return false;
	}

	listen(sock, 1);

	client->sock_pasv = sock;

	// enable event polling on pasv
	struct pollfd* ppfd = pollfd_array_add(&client->server->fds, client->server->nfds);

	if(ppfd != NULL)
	{
		ppfd->fd = client->sock_pasv;
		ppfd->events = POLLIN;

		++client->server->nfds;

		avltree_insert(client->server->clients, client->sock_pasv, client);
	}

	return true;
}

void ftpclient_event(struct FTPClient* client, int sock)
{
	if(sock == client->sock_data)
	{
		// event on data connection
		// call callback
		if(client->data_callback != NULL)
		{
			(*client->data_callback)(client);
		}
		else
		{
			// no callback, kill data connection
			ftpclient_data_end(client);
		}
	}
	else
	if(sock == client->sock_control)
	{
		// receive data from control socket
		ssize_t bytes = recv(sock, client->buf, client->bufsiz, 0);

		if(bytes <= 2)
		{
			// invalid data
			ftpclient_disconnect(client, sock);
			return;
		}

		client->buf[bytes - 2] = '\0';

		char* name;
		char* args;

		string_parsecmd(&name, &args, client->buf);
		string_toupper(name);

		if(!ftpcmd_call(client->server->commands, false, client, name, args))
		{
			ftpclient_send_message(client, 502, false, FTP_502);
		}
	}
	else
	if(sock == client->sock_pasv)
	{
		if(client->sock_data == -1)
		{
			client->sock_data = accept(client->sock_pasv, NULL, NULL);
		}

		ftpclient_disconnect(client, client->sock_pasv);
		client->sock_pasv = -1;
	}
}

void ftpclient_disconnect(struct FTPClient* client, int sock)
{
	nfds_t i = client->server->nfds;

	while(i--)
	{
		struct pollfd* pfd = &client->server->fds[i];

		if(pfd->fd == sock)
		{
			pollfd_array_remove(&client->server->fds, i, client->server->nfds--);
			break;
		}
	}

	avltree_remove(client->server->clients, sock);

	if(sock == client->sock_control)
	{
		// client disconnecting, remove data connections
		ftpclient_data_end(client);
		ftpserv_event_disconnect_call(client->server, client);
		ftpclient_destroy(client);

		sock = -1;
	}

	if(sock != -1)
	{
		shutdown(sock, SHUT_RDWR);
		close(sock);
	}
}

void ftpclient_destroy(struct FTPClient* client)
{
	if(client->sock_control != -1)
	{
		close(client->sock_control);
	}

	if(client->sock_data != -1)
	{
		close(client->sock_data);
	}

	if(client->sock_pasv != -1)
	{
		close(client->sock_pasv);
	}

	if(client->buf != client->server->buf)
	{
		free(client->buf);
	}

	if(client->handle != NULL)
	{
		if(client->handle->_dir)
		{
			ssftpFsClosedir(client->handle);
		}
		else
		{
			ssftpFsClose(client->handle);
		}

		client->handle = NULL;
	}

	free(client->addr);
	free(client);
}

void string_getpath(char* dst, const char* abspath, const char* relpath)
{
	dst[0] = '\0';

	if(relpath[0] == '/')
	{
		strcpy(dst, relpath);
	}
	else
	{
		strcpy(dst, abspath);
		strcat(dst, "/");
		strcat(dst, relpath);
	}

	// remove trailing slash
	if(strlen(dst) > 1)
	{
		char* last = strrchr(relpath, '/');

		if(last != NULL && *(last - 1) == '\0')
		{
			*last = '\0';
		}
	}
}

void string_getmode(char mode[11], struct stat* st)
{
	mode[0] = '?';

	if((st->st_mode & S_IFMT) == S_IFDIR)
	{
		mode[0] = 'd';
	}

	if((st->st_mode & S_IFMT) == S_IFREG)
	{
		mode[0] = '-';
	}
	
	if((st->st_mode & S_IFMT) == S_IFLNK)
	{
		mode[0] = 'l';
	}

	mode[1] = ((st->st_mode & S_IRUSR) ? 'r' : '-');
	mode[2] = ((st->st_mode & S_IWUSR) ? 'w' : '-');
	mode[3] = ((st->st_mode & S_IXUSR) ? 'x' : '-');
	mode[4] = ((st->st_mode & S_IRGRP) ? 'r' : '-');
	mode[5] = ((st->st_mode & S_IWGRP) ? 'w' : '-');
	mode[6] = ((st->st_mode & S_IXGRP) ? 'x' : '-');
	mode[7] = ((st->st_mode & S_IROTH) ? 'r' : '-');
	mode[8] = ((st->st_mode & S_IWOTH) ? 'w' : '-');
	mode[9] = ((st->st_mode & S_IXOTH) ? 'x' : '-');
	mode[10] = '\0';
}
