#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#ifdef __PSL1GHT__
#include <net/poll.h>
#define	TCP_NODELAY	0x01
#else
#include <sys/poll.h>
#include <netinet/tcp.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "server.h"
#include "response.h"
#include "util/array.h"

array(struct pollfd, pollfd_);
array(ftpclient_event_connect, connect_);
array(ftpclient_event_disconnect, disconnect_);

#ifdef __CELLOS_LV2__
#include "compat/cellos_lv2.h"
#endif

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh_exports.h"
#endif

int ftpserv_create(struct FTPServer* server, unsigned short port, struct FTPCommand* command)
{
	// Initialize struct member variables
	server->run = false;
	server->stop = false;

	server->sock = socket(AF_INET, SOCK_STREAM, 0);

	int optval = 1;
	setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	setsockopt(server->sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

	server->port = port;

	server->sin_addr.sin_family = AF_INET;
	server->sin_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	server->addr = (struct sockaddr*) &server->sin_addr;

	server->buf = malloc(BUFSIZ);
	server->bufsiz = BUFSIZ;

	server->fds = NULL;
	server->nfds = 0;

	server->clients = avltree_create();
	server->commands = command;

	server->connect_callbacks = NULL;
	server->nconnect_callbacks = 0;

	server->disconnect_callbacks = NULL;
	server->ndisconnect_callbacks = 0;

	return server->sock;
}

int ftpserv_run(struct FTPServer* server)
{
	if(server->run || server->sock == -1 || server->buf == NULL || server->clients == NULL)
	{
		return 1;
	}

	int ret = 0;

	server->stop = false;

	struct pollfd* spfd = pollfd_array_add(&server->fds, server->nfds);

	if(spfd != NULL)
	{
		spfd->fd = server->sock;
		spfd->events = POLLIN;

		++server->nfds;
	}
	else
	{
		return 1;
	}

	server->sin_addr.sin_port = htons(server->port);

	if(bind(server->sock, server->addr, sizeof(struct sockaddr)) == -1)
	{
		return 1;
	}

	listen(server->sock, 10);

	server->run = true;

	while(!server->stop)
	{
		int p = poll(server->fds, server->nfds, 500);

		if(p == 0)
		{
			// no events
			continue;
		}

		if(p == -1)
		{
			// error
			ret = 2;
			break;
		}

		// new event
		nfds_t i = server->nfds;

		while(p > 0 && i--)
		{
			struct pollfd* pfd = &server->fds[i];

			if(pfd == NULL)
			{
				continue;
			}

			if(pfd->revents)
			{
				// handle event
				--p;

				if(pfd->revents & POLLNVAL)
				{
					// remove invalid socket
					pollfd_array_remove(&server->fds, i, server->nfds--);
					continue;
				}

				if(pfd->fd == server->sock)
				{
					// new control connection
					int sock = accept(server->sock, NULL, NULL);

					if(sock == -1)
					{
						// error (ignore)
						continue;
					}

					int optval = 1;
					setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
					setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
					setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

					// initialize client
					struct FTPClient* client = ftpclient_create(sock, server, server->buf, server->bufsiz);

					if(client != NULL)
					{
						struct pollfd* cpfd = pollfd_array_add(&server->fds, server->nfds);

						if(cpfd != NULL)
						{
							cpfd->fd = sock;
							cpfd->events = POLLIN;

							++server->nfds;

							avltree_insert(server->clients, sock, client);
						}
						else
						{
							// cannot allocate memory
							ftpclient_send_message(client, 421, false, "Failed to allocate memory for event polling.");

							ftpclient_destroy(client);
							close(sock);
							continue;
						}

						ftpclient_send_message(client, 220, false, "ssFTP Ready.");
					}
					else
					{
						close(sock);
						continue;
					}
				}
				else
				{
					// find client
					struct AVLNode* n = avltree_search(server->clients, pfd->fd);

					if(n == NULL)
					{
						// orphan socket
						close(pfd->fd);
						pollfd_array_remove(&server->fds, i, server->nfds--);
						continue;
					}

					struct FTPClient* client = n->data_ptr;

					if(pfd->revents & (POLLERR|POLLHUP))
					{
						if(pfd->fd == client->sock_data)
						{
							ftpclient_data_end(client);
						}
						else
						{
							ftpclient_disconnect(client, pfd->fd);
						}

						continue;
					}

					#ifdef __CELLOS_LV2__
					// workaround for non-working POLLHUP
					if(pfd->revents & POLLIN && pfd->events & POLLOUT)
					{
						if(recv(pfd->fd, client->buf, client->bufsiz, MSG_PEEK) <= 0)
						{
							// 0-byte disconnect
							if(pfd->fd == client->sock_data)
							{
								ftpclient_data_end(client);
							}
							else
							{
								ftpclient_disconnect(client, pfd->fd);
							}

							continue;
						}
					}
					#endif

					ftpclient_event(client, pfd->fd);
				}
			}
		}
	}

	// broadcast shutdown
	while(server->clients->root != NULL)
	{
		struct FTPClient* client = server->clients->root->data_ptr;

		ftpclient_send_message(client, 421, false, "Server is shutting down.");
		ftpclient_disconnect(client, client->sock_control);
	}

	server->run = false;
	return ret;
}

void ftpserv_stop(struct FTPServer* server)
{
	server->stop = true;
}

void ftpserv_destroy(struct FTPServer* server)
{
	if(server->sock != -1)
	{
		close(server->sock);
		server->sock = -1;
	}

	free(server->buf);
	free(server->fds);

	avltree_destroy(server->clients);

	free(server->connect_callbacks);
	free(server->disconnect_callbacks);
}

void ftpserv_event_connect_register(struct FTPServer* server, ftpclient_event_connect callback)
{
	ftpclient_event_connect* connect_callback = connect_array_add(&server->connect_callbacks, server->nconnect_callbacks);

	if(connect_callback != NULL)
	{
		*connect_callback = callback;
		++server->nconnect_callbacks;
	}
}

void ftpserv_event_connect_unregister(struct FTPServer* server, ftpclient_event_connect callback)
{
	int n = server->nconnect_callbacks;

	while(n--)
	{
		if(server->connect_callbacks[n] == callback)
		{
			connect_array_remove(&server->connect_callbacks, n, --server->nconnect_callbacks);
			break;
		}
	}
}

void ftpserv_event_connect_call(struct FTPServer* server, struct FTPClient* client)
{
	int n = server->nconnect_callbacks;

	while(n--)
	{
		(*server->connect_callbacks[n])(client);
	}
}

void ftpserv_event_disconnect_register(struct FTPServer* server, ftpclient_event_disconnect callback)
{
	ftpclient_event_disconnect* disconnect_callback = disconnect_array_add(&server->disconnect_callbacks, server->ndisconnect_callbacks);

	if(disconnect_callback != NULL)
	{
		*disconnect_callback = callback;
		++server->ndisconnect_callbacks;
	}
}

void ftpserv_event_disconnect_unregister(struct FTPServer* server, ftpclient_event_disconnect callback)
{
	int n = server->ndisconnect_callbacks;

	while(n--)
	{
		if(server->disconnect_callbacks[n] == callback)
		{
			disconnect_array_remove(&server->disconnect_callbacks, n, --server->ndisconnect_callbacks);
			break;
		}
	}
}

void ftpserv_event_disconnect_call(struct FTPServer* server, struct FTPClient* client)
{
	int n = server->ndisconnect_callbacks;

	while(n--)
	{
		(*server->disconnect_callbacks[n])(client);
	}
}
