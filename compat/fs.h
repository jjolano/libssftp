#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

struct FTPFileHandle
{
	int _fd;
	void* _hptr;
	bool _dir;

	// unused in vanilla server
	unsigned short flags;
};

struct FTPFileHandle* ssftpFsOpen(const char* path, int oflags, mode_t mode);
size_t ssftpFsRead(struct FTPFileHandle* handle, void* buf, size_t nbytes);
size_t ssftpFsWrite(struct FTPFileHandle* handle, void* buf, size_t nbytes);
int ssftpFsClose(struct FTPFileHandle* handle);

struct FTPFileHandle* ssftpFsOpendir(const char* path);
struct FTPFileHandle* ssftpFsReaddir(struct FTPFileHandle* handle, struct dirent* entry);
int ssftpFsClosedir(struct FTPFileHandle* handle);

int ssftpFsStat(const char* path, struct stat* buf);
int ssftpFsFstat(struct FTPFileHandle* handle, struct stat* buf);

int ssftpFsMkdir(const char* path, mode_t mode);
int ssftpFsRename(const char* oldpath, const char* newpath);
int ssftpFsRmdir(const char* path);
int ssftpFsUnlink(const char* path);

int ssftpFsTruncate(const char* path, off_t length);
int ssftpFsFtruncate(struct FTPFileHandle* handle, off_t length);

int ssftpFsChmod(const char* path, mode_t mode);

int ssftpFsLseek(struct FTPFileHandle* handle, off_t offset, int whence);

#ifdef __cplusplus
}
#endif
