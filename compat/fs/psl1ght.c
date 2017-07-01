#include <stdlib.h>
#include <sys/file.h>
#include "compat/fs.h"

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpen(const char* path, int oflags, mode_t mode)
{

}

size_t __attribute__((weak)) ssftpFsRead(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{

}

size_t __attribute__((weak)) ssftpFsWrite(struct FTPFileHandle* handle, void* buf, size_t nbytes)
{

}

int __attribute__((weak)) ssftpFsClose(struct FTPFileHandle* handle)
{

}

struct FTPFileHandle* __attribute__((weak)) ssftpFsOpendir(const char* path)
{

}

struct FTPFileHandle* __attribute__((weak)) ssftpFsReaddir(struct FTPFileHandle* handle, struct dirent* entry)
{

}

int __attribute__((weak)) ssftpFsClosedir(struct FTPFileHandle* handle)
{

}

int __attribute__((weak)) ssftpFsStat(const char* path, struct stat* buf)
{

}

int __attribute__((weak)) ssftpFsFstat(struct FTPFileHandle* handle, struct stat* buf)
{

}

int __attribute__((weak)) ssftpFsMkdir(const char* path, mode_t mode)
{

}

int __attribute__((weak)) ssftpFsRename(const char* oldpath, const char* newpath)
{

}

int __attribute__((weak)) ssftpFsRmdir(const char* path)
{

}

int __attribute__((weak)) ssftpFsUnlink(const char* path)
{

}

int __attribute__((weak)) ssftpFsTruncate(const char* path, off_t length)
{

}

int __attribute__((weak)) ssftpFsFtruncate(struct FTPFileHandle* handle, off_t length)
{

}

int __attribute__((weak)) ssftpFsChmod(const char* path, mode_t mode)
{

}

int __attribute__((weak)) ssftpFsLseek(struct FTPFileHandle* handle, off_t offset, int whence)
{
	
}
