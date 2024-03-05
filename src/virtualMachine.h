#ifndef c_virtualMachine_h
#define c_virtualMachine_h

#include "object.h"
#include "table.h"
#include "value.h"

#define FRAMES_MAX 256
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

typedef struct {
  ObjFunction *function;
  uint8_t *ip;
  Value *slots; //< Point to VM's value stack of the first slot a function uses.
} CallFrame;

typedef struct {
  CallFrame frames[FRAMES_MAX];
  int frameCount;

  Value stack[STACK_MAX];
  Value *stackTop;
  Table globals;
  Table strings;
  Obj *objects;
} VirtualMachine;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VirtualMachine virtualMachine;

void initVirtualMachine();
void freeVirtualMachine();
InterpretResult interpret(const char *source);
void push(Value value);
Value pop();

#endif
