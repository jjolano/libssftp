#pragma once

#define	TCP_NODELAY	0x01

extern int closesocket(int socket);
#define close(a) closesocket(a)
