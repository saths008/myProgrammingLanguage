#include "memory.h"
#include "common.h"
#include "stdio.h"
#include "stdlib.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(pointer);
    return NULL;
  }
  void *newPtr = realloc(pointer, newSize);
  if (newPtr == NULL) {
    printf("Memory Allocation In Reallocate Failed.");
    exit(1);
  }
  return newPtr;
}
