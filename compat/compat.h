#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>

#ifndef __VITA__
#include <dirent.h>
#else
#include <psp2/io/dirent.h>
#include <psp2/io/fcntl.h>
#include <psp2/io/stat.h>

struct dirent
{
	long d_ino;
	char d_name[256];
};

#define INADDR_ANY SCE_NET_INADDR_ANY
#endif

#ifndef __USE_SELECT__
#ifdef __PSL1GHT__
#define	TCP_NODELAY	0x01
#include <net/poll.h>
#else
#include <sys/poll.h>
#include <netinet/tcp.h>
#endif
#else
#define	TCP_NODELAY	0x01
#include "selectpoll.h"
#endif

#ifdef __CELLOS_LV2__
#define poll(a,b,c) socketpoll(a,b,c)
#define close(a) socketclose(a)

#include <sys/fs_external.h>
#define PATH_MAX (CELL_FS_MAX_MP_LENGTH + CELL_FS_MAX_FS_PATH_LENGTH + 1)

#ifdef __CELLOS_PRX__
#include "cellos_prx/vsh_exports.h"
#endif
#endif

#ifdef __linux__
#include <linux/limits.h>
#else
#include <limits.h>
#endif

#ifndef PATH_MAX
#define PATH_MAX 1056
#endif
