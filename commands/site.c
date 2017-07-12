#include "site.h"

define_cmd2(ssftpCmdChmod)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	if(args == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		return;
	}

	char* chmod_args = strdup(args);
	char* arg = strtok(chmod_args, " ");

	if(arg == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		free(chmod_args);
		return;
	}

	mode_t mode = strtoul(arg, NULL, 8);

	arg = strtok(NULL, " ");

	if(arg == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		free(chmod_args);
		return;
	}

	char path[PATH_MAX];
	string_getpath(path, client->cwd, arg);

	if(ssftpFsChmod(path, mode) == 0)
	{
		ftpclient_send_message(client, 200, false, FTP_200);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}

	free(chmod_args);
}

define_cmd2(ssftpCmdStop)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	ftpserv_stop(client->server);
}

define_cmd2(ssftpCmdWrite)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	client->write_enabled = !client->write_enabled;

	ftpclient_send_message(client, 200, false, client->write_enabled ? "Writes are now enabled." : "Writes are now disabled.");
}
