#include <stdlib.h>
#include <string.h>
#include <cell/cell_fs.h>
#include <sys/memory.h>

#include "compat/fs.h"

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh/include/allocator.h"
#include "compat/cellos_prx/vsh/include/stdc.h"
#endif

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpen(const char* path, int oflags, mode_t mode)
{
	struct FTPFileHandle* ret = malloc(sizeof(struct FTPFileHandle));

	ret->_dir = false;
	ret->_hptr = NULL;
	ret->_data = 0;

	CellFsErrno fsret = cellFsOpen(path, oflags, &ret->_fd, NULL, 0);

	if(fsret != CELL_OK)
	{
		free(ret);
		return NULL;
	}

	ssftpFsChmod(path, mode);

	// create buffered io stuff
	sys_memory_container_t container = SYS_MEMORY_CONTAINER_ID_INVALID;
	CellFsErrno memret = sys_memory_container_create(&container, 1024 * 1024);

	if(memret == CELL_OK)
	{
		CellFsErrno ioret = cellFsSetIoBuffer(ret->_fd, BUFSIZ, CELL_FS_IO_BUFFER_PAGE_SIZE_64KB, container);

		if(ioret == CELL_OK)
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
		CellFsErrno fsret = cellFsRead(handle->_fd, buf, nbytes, &ret);

		if(fsret != CELL_OK)
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
		CellFsErrno fsret = cellFsWrite(handle->_fd, buf, nbytes, &ret);

		if(fsret != CELL_OK)
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
			sys_memory_container_t container = handle->_data;
			cellFsSetIoBuffer(handle->_fd, 0, CELL_FS_IO_BUFFER_PAGE_SIZE_64KB, container);
			sys_memory_container_destroy(container);
		}

		CellFsErrno fsret = cellFsClose(handle->_fd);

		if(fsret == CELL_OK)
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

	CellFsErrno fsret = cellFsOpendir(path, &ret->_fd);

	if(fsret != CELL_OK)
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
		CellFsDirent fsentry;
		uint64_t nread;

		CellFsErrno fsret = cellFsReaddir(handle->_fd, &fsentry, &nread);

		if(fsret == CELL_OK && nread > 0)
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
		CellFsErrno fsret = cellFsClosedir(handle->_fd);

		if(fsret == CELL_OK)
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

	CellFsStat sb;
	CellFsErrno fsret = cellFsStat(path, &sb);

	if(fsret == CELL_OK)
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

	CellFsStat sb;
	CellFsErrno fsret = cellFsFstat(handle->_fd, &sb);

	if(fsret == CELL_OK)
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

	CellFsErrno fsret = cellFsMkdir(path, mode);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsRename(const char* oldpath, const char* newpath)
{
	int ret = -1;

	CellFsErrno fsret = cellFsRename(oldpath, newpath);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsRmdir(const char* path)
{
	int ret = -1;

	CellFsErrno fsret = cellFsRmdir(path);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsUnlink(const char* path)
{
	int ret = -1;

	CellFsErrno fsret = cellFsUnlink(path);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsTruncate(const char* path, off_t length)
{
	int ret = -1;

	CellFsErrno fsret = cellFsTruncate(path, length);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsFtruncate(struct FTPFileHandle* handle, off_t length)
{
	int ret = -1;

	CellFsErrno fsret = cellFsFtruncate(handle->_fd, length);

	if(fsret == CELL_OK)
	{
		ret = 0;
	}

	return ret;
}

int __attribute__((weak)) ssftpFsChmod(const char* path, mode_t mode)
{
	int ret = -1;

	CellFsErrno fsret = cellFsChmod(path, mode);

	if(fsret == CELL_OK)
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
		CellFsErrno fsret = cellFsLseek(handle->_fd, offset, whence, &ret);

		if(fsret != CELL_OK)
		{
			ret = -1;
		}
	}

	return (off_t) ret;
}
