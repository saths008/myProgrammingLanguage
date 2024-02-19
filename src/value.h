#ifndef C_VALUE_H
#define C_VALUE_H
#include "common.h"

typedef enum {
  VAL_NIL,
  VAL_BOOL,

  VAL_NUMBER,
} ValueType;

typedef struct {
  ValueType type;
  union {
    bool boolean;
    double number;
  } as;
} Value;

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
