#include "base.h"
#include "base_data.h"

define_cmd2(ssftpCmdAbor)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	ftpclient_data_end(client);
	ftpclient_send_message(client, 226, false, FTP_226A);
}

define_cmd2(ssftpCmdAcct)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 202, false, FTP_202);
		return;
	}

	client->authorized = true;
	ftpclient_send_message(client, 230, false, FTP_230A);
}

define_cmd2(ssftpCmdAllo)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	ftpclient_send_message(client, 202, false, FTP_200);
}

define_cmd2(ssftpCmdCdup)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	char* last = strrchr(client->cwd, '/');

	if(last == client->cwd)
	{
		last++;
	}

	do
	{
		*last = '\0';
	} while(*(++last) != '\0');

	ftpclient_send_message(client, 250, false, FTP_250);
}

define_cmd2(ssftpCmdCwd)
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
	if(ssftpFsStat(path, &st) == 0 && (st.st_mode & S_IFMT) == S_IFDIR)
	{
		strcpy(client->cwd, path);
		ftpclient_send_message(client, 250, false, FTP_250);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdDele)
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

	if(ssftpFsUnlink(path) == 0)
	{
		ftpclient_send_message(client, 250, false, FTP_250);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdHelp)
{
	ftpclient_send_message(client, 214, false, "No help for you!");
}

define_cmd2(ssftpCmdList)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	if(client->handle != NULL)
	{
		ftpclient_send_message(client, 450, false, FTP_450);
		return;
	}

	client->handle = ssftpFsOpendir(client->cwd);

	if(client->handle == NULL)
	{
		ftpclient_send_message(client, 550, false, FTP_550);
		return;
	}

	if(ftpclient_data_start(client, ssftpDataList, true))
	{
		ftpclient_send_message(client, 150, false, FTP_150);
	}
	else
	{
		ssftpFsClosedir(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 425, false, FTP_425);
	}
}

define_cmd2(ssftpCmdMkd)
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

	if(ssftpFsMkdir(path, 0777) == 0)
	{
		char buf[PATH_MAX + 32];
		sprintf(buf, FTP_257, path);

		ftpclient_send_message(client, 257, false, buf);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdMode)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	char mode = args[0];

	if(mode >= 'a' && mode <= 'z')
	{
		mode -= 32;
	}

	// only stream is supported
	if(mode != 'S')
	{
		ftpclient_send_message(client, 504, false, FTP_504);
		return;
	}

	client->mode = mode;

	ftpclient_send_message(client, 200, false, FTP_200);
}

define_cmd2(ssftpCmdNlst)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}
	
	if(client->handle != NULL)
	{
		ftpclient_send_message(client, 450, false, FTP_450);
		return;
	}

	client->handle = ssftpFsOpendir(client->cwd);

	if(client->handle == NULL)
	{
		ftpclient_send_message(client, 550, false, FTP_550);
		return;
	}

	if(ftpclient_data_start(client, ssftpDataNlst, true))
	{
		ftpclient_send_message(client, 150, false, FTP_150);
	}
	else
	{
		ssftpFsClosedir(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 425, false, FTP_425);
	}
}

define_cmd2(ssftpCmdNoop)
{
	ftpclient_send_message(client, 200, false, FTP_200);
}

define_cmd2(ssftpCmdPass)
{
	if(client->authorized)
	{
		ftpclient_send_message(client, 202, false, FTP_202);
		return;
	}

	if(args == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		return;
	}

	if(client->username[0] == '\0')
	{
		strcpy(client->username, "anonymous");
	}

	// check username and password here

	client->authorized = true;

	char buf[64];
	sprintf(buf, FTP_230, client->username);

	ftpclient_send_message(client, 230, false, buf);
}

define_cmd2(ssftpCmdPasv)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	if(ftpclient_data_pasv(client))
	{
		struct sockaddr_in pasv_addr;
		socklen_t len = sizeof(pasv_addr);
		getsockname(client->sock_pasv, (struct sockaddr*) &pasv_addr, &len);

		char buf[64];
		sprintf(buf, FTP_227,
			((htonl(pasv_addr.sin_addr.s_addr) & 0xff000000) >> 24),
			((htonl(pasv_addr.sin_addr.s_addr) & 0x00ff0000) >> 16),
			((htonl(pasv_addr.sin_addr.s_addr) & 0x0000ff00) >>  8),
			(htonl(pasv_addr.sin_addr.s_addr) & 0x000000ff),
			((htons(pasv_addr.sin_port) & 0xff00) >> 8),
			(htons(pasv_addr.sin_port) & 0x00ff)
		);

		ftpclient_send_message(client, 227, false, buf);
	}
	else
	{
		ftpclient_send_message(client, 425, false, FTP_425);
	}
}

define_cmd2(ssftpCmdPort)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	unsigned short tuple[6];
	int argc = sscanf(args, "%hu,%hu,%hu,%hu,%hu,%hu",
		&tuple[0],
		&tuple[1],
		&tuple[2],
		&tuple[3],
		&tuple[4],
		&tuple[5]
	);

	if(argc != 6)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		return;
	}

	if(client->sock_data != -1)
	{
		ftpclient_send_message(client, 450, false, FTP_450);
		return;
	}

	client->actv = htons(tuple[4] << 8 | tuple[5]);
	ftpclient_send_message(client, 200, false, FTP_200);
}

define_cmd2(ssftpCmdPwd)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	char buf[PATH_MAX + 32];
	sprintf(buf, FTP_257, client->cwd);

	ftpclient_send_message(client, 257, false, buf);
}

define_cmd2(ssftpCmdQuit)
{
	ftpclient_send_message(client, 221, false, FTP_221);
	ftpclient_disconnect(client, client->sock_control);
}

define_cmd2(ssftpCmdRest)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	int64_t restval = 0;

	if(args[0] != '\0')
	{
		restval = atoll(args);
	}

	if(restval >= 0)
	{
		client->rest = restval;

		char buf[32];
		sprintf(buf, FTP_350, restval);

		ftpclient_send_message(client, 350, false, buf);
	}
	else
	{
		ftpclient_send_message(client, 554, false, FTP_554);
	}
}

define_cmd2(ssftpCmdRetr)
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

	if(client->handle != NULL)
	{
		ftpclient_send_message(client, 450, false, FTP_450);
		return;
	}
	
	char path[PATH_MAX];
	string_getpath(path, client->cwd, args);

	client->handle = ssftpFsOpen(path, O_RDONLY, 0777);

	if(client->handle == NULL)
	{
		ftpclient_send_message(client, 550, false, FTP_550);
		return;
	}

	ssftpFsLseek(client->handle, client->rest, SEEK_SET);
	client->rest = 0;

	if(ftpclient_data_start(client, ssftpDataRetr, true))
	{
		ftpclient_send_message(client, 150, false, FTP_150);
	}
	else
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 425, false, FTP_425);
	}
}

define_cmd2(ssftpCmdRmd)
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

	if(ssftpFsRmdir(path) == 0)
	{
		ftpclient_send_message(client, 250, false, FTP_250);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdRnfr)
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
		strcpy(client->rnfr, path);

		ftpclient_send_message(client, 350, false, FTP_350A);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}
}

define_cmd2(ssftpCmdRnto)
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

	if(ssftpFsRename(client->rnfr, path) == 0)
	{
		ftpclient_send_message(client, 250, false, FTP_250);
	}
	else
	{
		ftpclient_send_message(client, 550, false, FTP_550);
	}

	client->rnfr[0] = '\0';
}

define_cmd2(ssftpCmdSite)
{
	if(args == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		return;
	}

	char* site_data = strdup(args);

	char* site_name;
	char* site_args;

	string_parsecmd(&site_name, &site_args, site_data);
	string_toupper(site_name);

	if(!ftpcmd_call(client->server->commands, true, client, site_name, site_args))
	{
		ftpclient_send_message(client, 502, false, FTP_502);
	}

	free(site_data);
}

define_cmd2(ssftpCmdStat)
{
	char buf[64];

	ftpclient_send_message(client, 211, true, "Status");

	sprintf(buf, "Username: %s", client->username);
	ftpclient_send_message(client, 211, true, buf);

	sprintf(buf, "Authenticated: %d", client->authorized);
	ftpclient_send_message(client, 211, true, buf);

	sprintf(buf, "Total connections: %lu", client->server->nfds - 1);
	ftpclient_send_message(client, 211, true, buf);

	ftpclient_send_message(client, 211, false, "End.");
}

define_cmd2(ssftpCmdStor)
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
	
	if(client->handle != NULL)
	{
		ftpclient_send_message(client, 450, false, FTP_450);
		return;
	}
	
	char path[PATH_MAX];
	string_getpath(path, client->cwd, args);

	int oflags = O_WRONLY;

	struct stat st;
	if(ssftpFsStat(path, &st) != 0)
	{
		oflags |= O_CREAT;
	}

	if(strcmp(cmd, "APPE") == 0)
	{
		oflags |= O_APPEND;
	}
	else
	{
		if(client->rest == 0)
		{
			oflags |= O_TRUNC;
		}
	}

	client->handle = ssftpFsOpen(path, oflags, 0777);

	if(client->handle == NULL)
	{
		ftpclient_send_message(client, 550, false, FTP_550);
		return;
	}

	ssftpFsLseek(client->handle, client->rest, SEEK_SET);
	client->rest = 0;

	if(ftpclient_data_start(client, ssftpDataStor, false))
	{
		ftpclient_send_message(client, 150, false, FTP_150);
	}
	else
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 425, false, FTP_425);
	}
}

define_cmd2(ssftpCmdStru)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	char stru = args[0];

	if(stru >= 'a' && stru <= 'z')
	{
		stru -= 32;
	}

	// only file is supported
	if(stru != 'F')
	{
		ftpclient_send_message(client, 504, false, FTP_504);
		return;
	}

	client->stru = stru;

	ftpclient_send_message(client, 200, false, FTP_200);
}

define_cmd2(ssftpCmdSyst)
{
	ftpclient_send_message(client, 215, false, FTP_215);
}

define_cmd2(ssftpCmdType)
{
	if(!client->authorized)
	{
		ftpclient_send_message(client, 530, false, FTP_530);
		return;
	}

	char type = args[0];

	if(type >= 'a' && type <= 'z')
	{
		type -= 32;
	}

	// only binary is supported
	if(type != 'I')
	{
		ftpclient_send_message(client, 504, false, FTP_504);
		return;
	}

	client->type = type;

	ftpclient_send_message(client, 200, false, FTP_200);
}

define_cmd2(ssftpCmdUser)
{
	if(client->authorized)
	{
		ftpclient_send_message(client, 202, false, FTP_202);
		return;
	}

	if(args == NULL)
	{
		ftpclient_send_message(client, 501, false, FTP_501);
		return;
	}

	strcpy(client->username, args);

	char buf[64];
	sprintf(buf, FTP_331, client->username);

	ftpclient_send_message(client, 331, false, buf);
}

