#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>

#define array_add(array_ptr, count) \
({ \
	*array_ptr = realloc(*array_ptr, sizeof(typeof(*array_ptr)) * count); \
	array_ptr[count]; \
})

#define array_remove(array_ptr, index, count) \
({ \
	*array_ptr[index] = *array_ptr[count - 1]; \
	*array_ptr = realloc(*array_ptr, sizeof(typeof(*array_ptr)) * (count - 1)); \
})

#define array_remove_ordered(array_ptr, index, count) \
({ \
	memmove(&array_ptr[index], &array_ptr[index + 1], sizeof(typeof(*array_ptr)) * (count - index - 1)); \
	*array_ptr = realloc(*array_ptr, sizeof(typeof(*array_ptr)) * (count - 1)); \
})

#ifdef __cplusplus
}
#endif
