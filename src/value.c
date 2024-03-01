#include "value.h"
#include "object.h"
#include <string.h>
bool valuesEqual(Value a, Value b) {
  if (a.type != b.type)
    return false;
  switch (a.type) {
  case VAL_BOOL:
    return AS_BOOL(a) == AS_BOOL(b);
  case VAL_NIL:
    return true;
  case VAL_OBJ:
    return AS_OBJ(a) == AS_OBJ(b);
  case VAL_NUMBER:
    return AS_NUMBER(a) == AS_NUMBER(b);
  default:
    return false; // Unreachable.
  }
}
