#include "virtualMachine.h"
#include "bytecodeSeq.h"
#include "debug.h"

static void resetStack(VirtualMachine *virtualMachine) {
  virtualMachine->stackTop = virtualMachine->stack;
}

void initVirtualMachine(VirtualMachine *virtualMachine) {
  resetStack(virtualMachine);
}
void push(VirtualMachine *virtualMachine, Value value) {
  *virtualMachine->stackTop = value;
  virtualMachine->stackTop++;
}
Value pop(VirtualMachine *virtualMachine) {
  virtualMachine->stackTop--;
  return *virtualMachine->stackTop;
}

InterpretResultCode interpret(VirtualMachine *virtualMachine,
                              BytecodeSeq *bytecodeSeq) {
  virtualMachine->bytecodeSeq = bytecodeSeq;
  virtualMachine->instructionPointer = virtualMachine->bytecodeSeq->code;
  return run(virtualMachine);
}

InterpretResultCode run(VirtualMachine *virtualMachine) {

#define READ_BYTE() (*(virtualMachine->instructionPointer++))
#define READ_CONSTANT()                                                        \
  (virtualMachine->bytecodeSeq->constantPoolArray.values[READ_BYTE()])

#define BINARY_OP(op)                                                          \
  do {                                                                         \
    double b = pop(virtualMachine);                                            \
    double a = pop(virtualMachine);                                            \
    push(virtualMachine, a op b);                                              \
  } while (false)

  while (true) {
// Flag to disassemble the instruction
#ifdef DEBUG_INSTR_EXECUTION
    printf("          ");
    for (Value *slot = virtualMachine->stack; slot < virtualMachine->stackTop;
         slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");

    disassembleInstruction(virtualMachine->bytecodeSeq,
                           (int)(virtualMachine->instructionPointer -
                                 virtualMachine->bytecodeSeq->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_RETURN: {
      printValue(pop(virtualMachine));
      printf("\n");
      return INTERPRET_OK;
    }
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(virtualMachine, constant);
      break;
    }
    case OP_NEGATE: {
      push(virtualMachine, -pop(virtualMachine));
      break;
    }
    case OP_ADD: {
      BINARY_OP(+);
      break;
    }
    case OP_SUBTRACT: {
      BINARY_OP(-);
      break;
    }
    case OP_MULTIPLY: {
      BINARY_OP(*);
      break;
    }
    case OP_DIVIDE: {
      BINARY_OP(/);
      break;
    }
    }
#undef BINARY_OP
#undef READ_BYTE
#undef READ_CONSTANT
  }
}