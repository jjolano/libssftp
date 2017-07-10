#include <stdio.h>

#include "server/server.h"
#include "server/client.h"
#include "server/cmd.h"

#include "commands/base.h"

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

	// base
	register_cmd(&commands, "ABOR", ssftpCmdAbor);
	register_cmd(&commands, "ACCT", ssftpCmdAcct);
	register_cmd(&commands, "ALLO", ssftpCmdAllo);
	register_cmd(&commands, "CDUP", ssftpCmdCdup);
	register_cmd(&commands, "CWD", ssftpCmdCwd);
	register_cmd(&commands, "DELE", ssftpCmdDele);
	register_cmd(&commands, "HELP", ssftpCmdHelp);
	register_cmd(&commands, "LIST", ssftpCmdList);
	register_cmd(&commands, "MKD", ssftpCmdMkd);
	register_cmd(&commands, "MODE", ssftpCmdMode);
	register_cmd(&commands, "NLST", ssftpCmdNlst);
	register_cmd(&commands, "NOOP", ssftpCmdNoop);
	register_cmd(&commands, "PASS", ssftpCmdPass);
	register_cmd(&commands, "PASV", ssftpCmdPasv);
	register_cmd(&commands, "PORT", ssftpCmdPort);
	register_cmd(&commands, "PWD", ssftpCmdPwd);
	register_cmd(&commands, "QUIT", ssftpCmdQuit);
	register_cmd(&commands, "REST", ssftpCmdRest);
	register_cmd(&commands, "RETR", ssftpCmdRetr);
	register_cmd(&commands, "RMD", ssftpCmdRmd);
	register_cmd(&commands, "RNFR", ssftpCmdRnfr);
	register_cmd(&commands, "RNTO", ssftpCmdRnto);
	register_cmd(&commands, "SITE", ssftpCmdSite);
	register_cmd(&commands, "STAT", ssftpCmdStat);
	register_cmd(&commands, "STOR", ssftpCmdStor);
	register_cmd(&commands, "STRU", ssftpCmdStru);
	register_cmd(&commands, "SYST", ssftpCmdSyst);
	register_cmd(&commands, "TYPE", ssftpCmdType);
	register_cmd(&commands, "USER", ssftpCmdUser);

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
