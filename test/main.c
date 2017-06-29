#include "server/server.h"
#include "server/cmd.h"

int main(void)
{
	struct FTPCommand commands;
	struct FTPServer server;

	ftpcmd_create(&commands);

	ftpserv_create(&server, 21, &commands);

	ftpserv_run(&server);
	ftpserv_destroy(&server);

	return 0;
}
