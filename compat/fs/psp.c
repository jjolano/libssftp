#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "compat/fs.h"

#if _POSIX_C_SOURCE >= 200112L
#define ftell(a) ftello(a)
#define fseek(a,b,c) fseeko(a,b,c)
#endif

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpen(const char* path, int oflags, mode_t mode)
{
	struct FTPFileHandle* ret = malloc(sizeof(struct FTPFileHandle));

	ret->_dir = false;

	FILE* fp = NULL;

	if(oflags & O_CREAT)
	{
		fp = fopen(path, "wb");
	}
	else
	if(oflags & O_APPEND)
	{
		fp = fopen(path, "ab+");
	}
	else
	if(oflags & O_RDONLY)
	{
		fp = fopen(path, "r");
	}
	else
	if(oflags & O_TRUNC)
	{
		fp = fopen(path, "wb+");
	}
	else
	{
		fp = fopen(path, "rb+");
	}

	if(fp == NULL)
	{
		free(ret);
		return NULL;
	}

	ret->_hptr = fp;
	ret->_fd = fileno(fp);

	//fchmod(ret->_fd, mode);
	//chmod(path, mode);
	ssftpFsChmod(path, mode);

	return ret;
}

size_t __attribute__((weak)) ssftpFsRead(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{
	size_t ret = 0;

	if(!handle->_dir)
	{
		ret = fread(buf, sizeof(char), nbytes, handle->_hptr);
	}

	return ret;
}

size_t __attribute__((weak)) ssftpFsWrite(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{
	size_t ret = 0;

	if(!handle->_dir)
	{
		ret = fwrite(buf, sizeof(char), nbytes, handle->_hptr);
	}

	return ret;
}

int __attribute__((weak)) ssftpFsClose(struct FTPFileHandle* handle)
{
	int ret = -1;

	if(!handle->_dir)
	{
		ret = fclose(handle->_hptr);
		free(handle);
	}

	return ret;
}

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpendir(const char* path)
{
	struct FTPFileHandle* ret = malloc(sizeof(struct FTPFileHandle));

	ret->_dir = true;

	DIR* dirp = opendir(path);

	if(dirp == NULL)
	{
		free(ret);
		return NULL;
	}

	ret->_hptr = dirp;
	ret->_fd = -1;

	return ret;
}

struct FTPFileHandle* __attribute__((weak)) ssftpFsReaddir(struct FTPFileHandle* handle, struct dirent* entry)
{
	struct FTPFileHandle* ret = NULL;

	if(handle->_dir)
	{
		struct dirent* _entry = readdir(handle->_hptr);

		if(_entry != NULL)
		{
			ret = handle;
			strcpy(entry->d_name, _entry->d_name);
			//entry->d_ino = _entry->d_ino;
		}
	}

	return ret;
}

int __attribute__((weak)) ssftpFsClosedir(struct FTPFileHandle* handle)
{
	int ret = -1;

	if(handle->_dir)
	{
		ret = closedir(handle->_hptr);
		free(handle);
	}

	return ret;
}

int __attribute__((weak)) ssftpFsStat(const char* path, struct stat* buf)
{
	return stat(path, buf);
}

int __attribute__((weak)) ssftpFsFstat(struct FTPFileHandle* handle, struct stat* buf)
{
	return fstat(handle->_fd, buf);
}

int __attribute__((weak)) ssftpFsMkdir(const char* path, mode_t mode)
{
	return mkdir(path, mode);
}

int __attribute__((weak)) ssftpFsRename(const char* oldpath, const char* newpath)
{
	return rename(oldpath, newpath);
}

int __attribute__((weak)) ssftpFsRmdir(const char* path)
{
	return rmdir(path);
}

int __attribute__((weak)) ssftpFsUnlink(const char* path)
{
	return unlink(path);
}

int __attribute__((weak)) ssftpFsTruncate(const char* path, off_t length)
{
	//return truncate(path, length);
	return -1;
}

int __attribute__((weak)) ssftpFsFtruncate(struct FTPFileHandle* handle, off_t length)
{
	//return ftruncate(handle->_fd, length);
	return -1;
}

int __attribute__((weak)) ssftpFsChmod(const char* path, mode_t mode)
{
	return chmod(path, mode);
}

off_t __attribute__((weak)) ssftpFsLseek(struct FTPFileHandle* handle, off_t offset, int whence)
{
	off_t ret = -1;

	if(!handle->_dir)
	{
		ret = fseek(handle->_hptr, offset, whence);
	}

	return ret;
}
