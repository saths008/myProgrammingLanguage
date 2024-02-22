#include "memory.h"
#include "common.h"
#include "object.h"
#include "stdio.h"
#include "stdlib.h"
#include "value.h"
#include "virtualMachine.h"

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
static void freeObject(Obj *object) {
  switch (object->type) {
  case OBJ_STRING: {
    ObjString *string = (ObjString *)object;
    FREE_ARRAY(char, string->chars, string->length + 1);
    FREE(ObjString, object);
    break;
  }
  }
}

void freeObjects() {
  Obj *object = virtualMachine.objects;
  while (object != NULL) {
    Obj *next = object->next;
    freeObject(object);
    object = next;
  }
}
