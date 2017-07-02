#include <stdio.h>

#include "server/server.h"
#include "server/client.h"
#include "server/cmd.h"

void client_connect(struct FTPClient* client)
{
	printf("Client connected! (fd: %d)\n", client->sock_control);
}

void client_disconnect(struct FTPClient* client)
{
	printf("Client disconnected! (fd: %d)\n", client->sock_control);
}

int main(void)
{
	struct FTPCommand commands;
	struct FTPServer server;

	ftpcmd_create(&commands);
	ftpserv_create(&server, 21, &commands);

	ftpserv_event_connect_register(&server, client_connect);
	ftpserv_event_disconnect_register(&server, client_disconnect);

	printf("Server running!\n");

	ftpserv_run(&server);

	printf("Server stopped!\n");

	ftpserv_destroy(&server);
	ftpcmd_destroy(&commands);

	printf("Resources freed!\n");

	return 0;
}
