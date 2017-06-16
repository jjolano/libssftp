#pragma once

#define poll(a,b,c) socketpoll(a,b,c)
#define close(a) socketclose(a)
