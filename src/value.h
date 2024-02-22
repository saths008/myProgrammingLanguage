#ifndef C_VALUE_H
#define C_VALUE_H
#include "common.h"
typedef struct Obj Obj;
typedef struct ObjString ObjString;
typedef enum {
  VAL_NIL,
  VAL_BOOL,

  VAL_OBJ,
  VAL_NUMBER,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
    Obj *obj;
  } as;
} Value;
#define IS_OBJ(value) ((value).type == VAL_OBJ)
#define AS_OBJ(value) ((value).as.obj)
#define OBJ_VAL(object) ((Value){VAL_OBJ, {.obj = (Obj *)object}})
bool valuesEqual(Value a, Value b);
// From C value to Value
#define BOOL_VAL(value) ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL ((Value){VAL_NIL, {.number = 0}})
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})

#define AS_BOOL(value) ((value).as.boolean)
#define AS_NUMBER(value) ((value).as.number)

#define IS_BOOL(value) ((value).type == VAL_BOOL)
#define IS_NIL(value) ((value).type == VAL_NIL)
#define IS_NUMBER(value) ((value).type == VAL_NUMBER)

#endif
