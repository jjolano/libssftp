#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/memory.h>
#include "compat/fs.h"

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpen(const char* path, int oflags, mode_t mode)
{
	struct FTPFileHandle* ret = malloc(sizeof(struct FTPFileHandle));

	ret->_dir = false;
	ret->_hptr = NULL;
	ret->_data = 0;

	s32 fsret = sysFsOpen(path, oflags, &ret->_fd, NULL, 0);

	if(fsret != 0)
	{
		free(ret);
		return NULL;
	}

	ssftpFsChmod(path, mode);

	// create buffered io stuff
	sys_mem_container_t container = 0xFFFFFFFFU;
	s32 memret = sysMemContainerCreate(&container, 1024 * 1024);

	if(memret == 0)
	{
		s32 ioret = sysFsSetIoBuffer(ret->_fd, BUFSIZ, SYS_MEMORY_PAGE_SIZE_64K, container);

		if(ioret == 0)
		{
			ret->_data = container;
		}
	}

	return ret;
}

size_t __attribute__((weak)) ssftpFsRead(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{
	uint64_t ret = 0;

	if(!handle->_dir)
	{
		s32 fsret = sysFsRead(handle->_fd, buf, nbytes, &ret);

		if(fsret != 0)
		{
			ret = 0;
		}
	}

	return (size_t) ret;
}

size_t __attribute__((weak)) ssftpFsWrite(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{
	uint64_t ret = 0;

	if(!handle->_dir)
	{
		s32 fsret = sysFsWrite(handle->_fd, buf, nbytes, &ret);

		if(fsret != 0)
		{
			ret = 0;
		}
	}

	return (size_t) ret;
}

int __attribute__((weak)) ssftpFsClose(struct FTPFileHandle* handle)
{
	int ret = -1;

	if(!handle->_dir)
	{
		if(handle->_data != 0)
		{
			sys_mem_container_t container = handle->_data;
			sysFsSetIoBuffer(handle->_fd, 0, SYS_MEMORY_PAGE_SIZE_64K, container);
			sysMemContainerDestroy(container);
		}

		s32 fsret = sysFsClose(handle->_fd);

		if(fsret == 0)
		{
			ret = 0;
		}

		free(handle);
	}

	return ret;
}

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpendir(const char* path)
{
	struct FTPFileHandle* ret = malloc(sizeof(struct FTPFileHandle));

	ret->_dir = true;
	ret->_hptr = NULL;

	s32 fsret = sysFsOpendir(path, &ret->_fd);

	if(fsret != 0)
	{
		free(ret);
		return NULL;
	}

	return ret;
}

struct FTPFileHandle* __attribute__((weak)) ssftpFsReaddir(struct FTPFileHandle* handle, struct dirent* entry)
{
	struct FTPFileHandle* ret = NULL;

	if(handle->_dir)
	{
		sysFSDirent fsentry;
		uint64_t nread;

		s32 fsret = sysFsReaddir(handle->_fd, &fsentry, &nread);

		if(fsret == 0 && nread > 0)
		{
			ret = handle;
			strcpy(entry->d_name, fsentry.d_name);
			entry->d_ino = 1;
		}
	}

	return ret;
}

int __attribute__((weak)) ssftpFsClosedir(struct FTPFileHandle* handle)
{
	int ret = -1;

	if(handle->_dir)
	{
		s32 fsret = sysFsClosedir(handle->_fd);

		if(fsret == 0)
		{
			ret = 0;
		}

		free(handle);
	}

	return ret;
}

int __attribute__((weak)) ssftpFsStat(const char* path, struct stat* buf)
{
	int ret = -1;

	sysFSStat sb;
	s32 fsret = sysFsStat(path, &sb);

	if(fsret == 0)
	{
		ret = 0;

		buf->st_ino = 1;
		buf->st_mode = sb.st_mode;
		buf->st_uid = sb.st_uid;
		buf->st_gid = sb.st_gid;
		buf->st_atime = sb.st_atime;
		buf->st_mtime = sb.st_mtime;
		buf->st_ctime = sb.st_ctime;
		buf->st_size = sb.st_size;
		buf->st_blksize = sb.st_blksize;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsFstat(struct FTPFileHandle* handle, struct stat* buf)
{
	int ret = -1;

	sysFSStat sb;
	s32 fsret = sysFsFstat(handle->_fd, &sb);

	if(fsret == 0)
	{
		ret = 0;

		buf->st_ino = 1;
		buf->st_mode = sb.st_mode;
		buf->st_uid = sb.st_uid;
		buf->st_gid = sb.st_gid;
		buf->st_atime = sb.st_atime;
		buf->st_mtime = sb.st_mtime;
		buf->st_ctime = sb.st_ctime;
		buf->st_size = sb.st_size;
		buf->st_blksize = sb.st_blksize;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsMkdir(const char* path, mode_t mode)
{
	int ret = -1;

	s32 fsret = sysFsMkdir(path, mode);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsRename(const char* oldpath, const char* newpath)
{
	int ret = -1;

	s32 fsret = sysLv2FsRename(oldpath, newpath);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsRmdir(const char* path)
{
	int ret = -1;

	s32 fsret = sysFsRmdir(path);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsUnlink(const char* path)
{
	int ret = -1;

	s32 fsret = sysFsUnlink(path);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsTruncate(const char* path, off_t length)
{
	int ret = -1;

	s32 fsret = sysLv2FsTruncate(path, length);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsFtruncate(struct FTPFileHandle* handle, off_t length)
{
	int ret = -1;

	s32 fsret = sysLv2FsFtruncate(handle->_fd, length);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsChmod(const char* path, mode_t mode)
{
	int ret = -1;

	s32 fsret = sysFsChmod(path, mode);

	if(fsret == 0)
	{
		ret = 0;
	}

	return ret;
}

off_t __attribute__((weak)) ssftpFsLseek(struct FTPFileHandle* handle, off_t offset, int whence)
{
	uint64_t ret = -1;

	if(!handle->_dir)
	{
		s32 fsret = sysFsLseek(handle->_fd, offset, whence, &ret);

		if(fsret != 0)
		{
			ret = -1;
		}
	}

	return (off_t) ret;
}
