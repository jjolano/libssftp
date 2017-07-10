#include <time.h>
#include <inttypes.h>
#include "ext.h"

define_cmd2(ssftpCmdSize)
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

	char path[PATH_MAX];
	string_getpath(path, client->cwd, args);

	struct stat st;
	if(ssftpFsStat(path, &st) == 0)
	{
		char buf[64];
		sprintf(buf, "%jd", (uintmax_t) st.st_size);

		ftpclient_send_message(client, 213, false, buf);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdMdtm)
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

	char path[PATH_MAX];
	string_getpath(path, client->cwd, args);

	struct stat st;
	if(ssftpFsStat(path, &st) == 0)
	{
		char buf[64];
		strftime(buf, 63, "%Y%m%d%H%M%S", gmtime(&st.st_mtime));

		ftpclient_send_message(client, 213, false, buf);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}
