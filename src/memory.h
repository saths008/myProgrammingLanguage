#ifndef c_memory_h
#define c_memory_h

#include "common.h"

#define CALCULATE_NEW_CAPACITY(capacity) ((capacity) < 10 ? 10 : (capacity) * 2)
#define GROW_ARRAY(type, startPtr, oldCapacity, newCapacity)                   \
  ((type *)reallocate(startPtr, sizeof(type) * oldCapacity,                    \
                      sizeof(type) * newCapacity))
#define FREE_ARRAY(type, startPtr, oldCapacity)                                \
  (reallocate(startPtr, sizeof(type) * oldCapacity, 0))
// Custom reallocate memory function
void *reallocate(void *pointer, size_t oldSize, size_t newSize);
#endif
