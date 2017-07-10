#include <time.h>
#include <inttypes.h>
#include "base_data.h"

define_data2(ssftpDataList)
{
	struct dirent entry;
	if(ssftpFsReaddir(client->handle, &entry) == NULL)
	{
		ssftpFsClosedir(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 226, false, FTP_226);
		ftpclient_data_end(client);
		return;
	}

	char path[PATH_MAX];
	string_getpath(path, client->cwd, entry.d_name);

	struct stat st;
	if(ssftpFsStat(path, &st) == 0)
	{
		char mode[11];
		string_getmode(mode, &st);

		time_t rawtime;
		time(&rawtime);

		struct tm* curtime = gmtime(&rawtime);
		struct tm* modtime = gmtime(&st.st_mtime);

		bool useyear = (curtime->tm_year != modtime->tm_year || curtime->tm_mon - modtime->tm_mon >= 6);

		char tstr[16];
		strftime(tstr, 15, useyear ? "%b %e  %Y" : "%b %e %H:%M", modtime);

		int len = sprintf(client->buf,
			"%s %3lu %-10d %-10d %10jd %s %s\r\n",
			mode, st.st_ino, st.st_uid, st.st_gid, (uintmax_t) st.st_size, tstr, entry.d_name
		);

		ssize_t sent = send(client->sock_data, client->buf, (size_t) len, 0);

		if(sent == -1 || sent < len)
		{
			ssftpFsClosedir(client->handle);
			client->handle = NULL;

			ftpclient_send_message(client, 451, false, FTP_451);
			ftpclient_data_end(client);
		}
	}
}

define_data2(ssftpDataNlst)
{
	struct dirent entry;
	if(ssftpFsReaddir(client->handle, &entry) == NULL)
	{
		ssftpFsClosedir(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 226, false, FTP_226);
		ftpclient_data_end(client);
		return;
	}

	char path[PATH_MAX];
	string_getpath(path, client->cwd, entry.d_name);

	struct stat st;
	if(ssftpFsStat(path, &st) == 0)
	{
		int len = sprintf(client->buf, "%s\r\n", entry.d_name);
		ssize_t sent = send(client->sock_data, client->buf, (size_t) len, 0);

		if(sent == -1 || sent < len)
		{
			ssftpFsClosedir(client->handle);
			client->handle = NULL;

			ftpclient_send_message(client, 451, false, FTP_451);
			ftpclient_data_end(client);
		}
	}
}

define_data2(ssftpDataRetr)
{
	size_t nread = ssftpFsRead(client->handle, client->buf, BUFSIZ);

	if(nread == 0)
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 226, false, FTP_226);
		ftpclient_data_end(client);
		return;
	}

	ssize_t sent = send(client->sock_data, client->buf, nread, 0);

	if(sent == -1 || sent < nread)
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 451, false, FTP_451);
		ftpclient_data_end(client);
	}
}

define_data2(ssftpDataStor)
{
	ssize_t nread = recv(client->sock_data, client->buf, BUFSIZ, 0);

	if(nread == 0)
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 226, false, FTP_226);
		ftpclient_data_end(client);
		return;
	}

	if(nread == -1)
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 451, false, FTP_451);
		ftpclient_data_end(client);
		return;
	}

	size_t written = ssftpFsWrite(client->handle, client->buf, nread);

	if(written < nread)
	{
		ssftpFsClose(client->handle);
		client->handle = NULL;

		ftpclient_send_message(client, 452, false, FTP_452);
		ftpclient_data_end(client);
	}
}
