#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "compat/compat.h"

#define array(type, prefix) \
type* prefix##array_add(type** array_ptr, size_t count) \
{ \
	*array_ptr = realloc(*array_ptr, sizeof(type) * (count + 1)); \
	return &((*array_ptr)[count]); \
} \
void prefix##array_remove(type** array_ptr, size_t index, size_t count) \
{ \
	(*array_ptr)[index] = (*array_ptr)[count - 1]; \
	*array_ptr = realloc(*array_ptr, sizeof(type) * (count - 1)); \
}

#define define_array(type, prefix) \
extern type* prefix##array_add(type** array_ptr, size_t count); \
extern type* prefix##array_remove(type** array_ptr, size_t index, size_t count);

#ifdef __cplusplus
}
#endif
