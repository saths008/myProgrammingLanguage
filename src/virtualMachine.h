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
typedef struct {
  BytecodeSeq *bytecodeSeq;
  uint8_t *instructionPointer; // points to the next instruction to be executed
} VirtualMachine;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResultCode;

void initVirtualMachine(VirtualMachine *virtualMachine);

void freeVirtualMachine(VirtualMachine *virtualMachine);

InterpretResultCode interpret(VirtualMachine *virtualMachine,
                              BytecodeSeq *bytecodeSeq);

InterpretResultCode run(VirtualMachine *virtualMachine);

#endif // !c_virtualMachine_h
