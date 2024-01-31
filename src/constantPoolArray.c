#include "constantPoolArray.h"
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>

void initConstantPoolArray(ConstantPoolArray *constantPoolArray) {
  constantPoolArray->capacity = 0;
  constantPoolArray->count = 0;
  constantPoolArray->values = NULL;
}

void writeConstantPoolArray(ConstantPoolArray *constantPoolArray, Value value) {
  if ((constantPoolArray->count + 1) > constantPoolArray->capacity) {

    int oldCapacity = constantPoolArray->capacity;
    int newCapacity = CALCULATE_NEW_CAPACITY(oldCapacity);
    constantPoolArray->values =
        GROW_ARRAY(Value, constantPoolArray->values, oldCapacity, newCapacity);
    constantPoolArray->capacity = newCapacity;
  }
  constantPoolArray->values[constantPoolArray->count] = value;
  constantPoolArray->count++;
}

void freeConstantPoolArray(ConstantPoolArray *constantPoolArray) {
  FREE_ARRAY(Value, constantPoolArray->values, constantPoolArray->capacity);
  // memory is left in an indeterminate state so initConstantPoolArray to reset
  // fields.
  initConstantPoolArray(constantPoolArray);
}
