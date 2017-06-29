#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#ifdef __CELLOS_PRX__
#include "compat/cellos_prx/vsh/include/allocator.h"
#include "compat/cellos_prx/vsh/include/stdc.h"
#endif

#define array(type, prefix) \
inline type* prefix##array_add(type** array_ptr, size_t count) \
{ \
	*array_ptr = realloc(*array_ptr, sizeof(type) * (count + 1)); \
	return &((*array_ptr)[count]); \
} \
inline void prefix##array_remove(type** array_ptr, size_t index, size_t count) \
{ \
	(*array_ptr)[index] = (*array_ptr)[count - 1]; \
	*array_ptr = realloc(*array_ptr, sizeof(type) * (count - 1)); \
}

#ifdef __cplusplus
}
#endif
