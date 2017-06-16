#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>

#include "client.h"
#include "util/array.h"
#include "util/avl.h"

#ifdef __CELLOS_LV2__
#include "compat/cellos_lv2.h"
#endif

#ifdef __CELLOS_PRX__

#endif

struct FTPClient* ftpclient_create(int sock, struct FTPServer* server, char* buf, int bufsiz, struct sockaddr* addr, socklen_t addrlen)
{
	struct FTPClient* client = malloc(sizeof(struct FTPClient));

	if(client != NULL)
	{
		client->server = server;

		client->sock_control = sock;
		client->sock_data = -1;
		client->sock_pasv = -1;

		client->addr = malloc(sizeof(struct sockaddr));
		memcpy(client->addr, addr, addrlen);
		client->addrlen = addrlen;

		client->buf = buf;
		client->bufsiz = bufsiz;

		client->data_callback = NULL;

		client->fp = NULL;
		client->dirp = NULL;

		client->cwd[0] = '\0';
		client->username[0] = '\0';
		client->rest = 0;
		client->rnfr = NULL;
		client->actv = 20;
		client->authorized = false;

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

			close(client->sock_pasv);
			client->sock_pasv = -1;

			if(p <= 0 || client->sock_data == -1)
			{
				return false;
			}
		}
	}
	
	// prevent malicious command usage
	if(client->data_callback != NULL)
	{
		return false;
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
	struct pollfd* fd = array_add(&client->server->fds, client->server->nfds);

	if(fd != NULL)
	{
		fd->fd = client->sock_data;
		fd->events = events;

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
	if(client->sock_data != -1)
	{
		if(client->fp != NULL)
		{
			fclose(client->fp);
			client->fp = NULL;
		}

		if(client->dirp != NULL)
		{
			closedir(client->dirp);
			client->dirp = NULL;
		}

		ftpclient_disconnect(client, client->sock_data);

		client->sock_data = -1;
		client->data_callback = NULL;
	}

	if(client->sock_pasv != -1)
	{
		close(client->sock_pasv);
		client->sock_pasv = -1;
	}
}

bool ftpclient_data_pasv(struct FTPClient* client)
{
	if(client->sock_data != -1)
	{
		return false;
	}

	if(client->sock_pasv != -1)
	{
		close(client->sock_pasv);
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

		if(!ftpcmd_call(client->server->commands, false, client, name, args))
		{
			ftpclient_send_message(client, 502, false, "Command not implemented.");
		}
	}
}

void ftpclient_disconnect(struct FTPClient* client, int sock)
{
	if(sock == client->sock_control)
	{
		// client disconnecting, remove data connections
		ftpclient_data_end(client);
		ftpserv_event_disconnect_call(client->server, client);
	}

	shutdown(sock, SHUT_RDWR);
	close(sock);

	nfds_t i = client->server->nfds;

	while(i--)
	{
		struct pollfd* pfd = &client->server->fds[i];

		if(pfd->fd == sock)
		{
			array_remove(&client->server->fds, i, client->server->nfds--);
			break;
		}
	}

	avltree_remove(client->server->clients, sock);
}

void ftpclient_destroy(struct FTPClient* client, bool freebuf)
{
	close(client->sock_control);
	close(client->sock_data);
	close(client->sock_pasv);

	free(client->addr);

	if(freebuf)
	{
		free(client->buf);
	}

	if(client->fp != NULL)
	{
		fclose(client->fp);
	}

	if(client->dirp != NULL)
	{
		closedir(client->dirp);
	}

	free(client->rnfr);
	free(client);
}
