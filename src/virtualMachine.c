#include "virtualMachine.h"

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

  while (true) {
// Flag to disassemble the instruction
#ifdef DEBUG_INSTR_EXECUTION
    disassembleInstruction(virtualMachine->bytecodeSeq,
                           (int)(virtualMachine->instructionPointer -
                                 virtualMachine->bytecodeSeq->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_RETURN:
      return INTERPRET_OK;
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      printf("%g", constant);
      printf("\n");
      break;
    }
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
}
