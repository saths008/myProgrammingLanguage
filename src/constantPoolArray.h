#ifndef c_constantPoolArray_h
#define c_constantPoolArray_h

#include "common.h"

typedef double Value;
typedef struct {
  int capacity;
  int count;
  Value *values;
} ConstantPoolArray;

void initConstantPoolArray(ConstantPoolArray *constantPoolArray);
void writeConstantPoolArray(ConstantPoolArray *constantPoolArray, Value value);
void freeConstantPoolArray(ConstantPoolArray *constantPoolArray);

#endif // !c_constantPoolArray_h
