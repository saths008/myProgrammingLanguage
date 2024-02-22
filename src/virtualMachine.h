#ifndef c_virtualMachine_h
#define c_virtualMachine_h
#include "bytecodeSeq.h"
#include "common.h"
#include "debug.h"
#include <stdio.h>

/**
 * @brief Virtual machine, takes in a bytecode sequence and executes it.
 *
 */
#define STACK_MAX 256
typedef struct {
  BytecodeSeq *bytecodeSeq;
  uint8_t *instructionPointer; // points to the next instruction to be executed
  Value *stackTop;             // points to the next empty slot in the stack
  Value stack[STACK_MAX];
  Obj *objects;
} VirtualMachine;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResultCode;

extern VirtualMachine virtualMachine;
void push(Value value);
Value pop();
void initVirtualMachine();

void freeVirtualMachine();

InterpretResultCode interpret(const char *sourceCode);

InterpretResultCode run();

#endif // !c_virtualMachine_h
