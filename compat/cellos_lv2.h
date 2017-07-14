#pragma once

#define poll(a,b,c) socketpoll(a,b,c)
#define close(a) socketclose(a)

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh_exports.h"
#endif
