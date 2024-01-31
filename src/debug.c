#include "debug.h"
#include "bytecodeSeq.h"
#include <stdint.h>
#include <stdio.h>
static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}
static int constantInstruction(const char *name, BytecodeSeq *bytecodeSeq,
                               int offset) {
  uint8_t constantIndexInConstantPool = bytecodeSeq->code[offset + 1];
  printf("%-16s %4d '", name, constantIndexInConstantPool);
  printValue(
      bytecodeSeq->constantPoolArray.values[constantIndexInConstantPool]);
  printf("'\n");
  return offset + 2;
}
void printValue(Value value) { printf("%g", value); }
void disassembleBytecodeSeq(BytecodeSeq *bytecodeSeq, const char *name) {

  printf("== %s ==\n", name);
  for (int offset = 0; offset < bytecodeSeq->count;) {
    offset = disassembleInstruction(bytecodeSeq, offset);
  }
}
int disassembleInstruction(BytecodeSeq *bytecodeSeq, int offset) {
  printf("%04d ", offset);
  if (offset > 0 && bytecodeSeq->lineNumbers[offset] ==
                        bytecodeSeq->lineNumbers[offset - 1]) {
    printf("   SAME ");
  } else {
    printf("%4d ", bytecodeSeq->lineNumbers[offset]);
  }
  uint8_t instruction = bytecodeSeq->code[offset];
  switch (instruction) {
  case (OP_RETURN):
    return simpleInstruction("OP_RETURN", offset);

  case (OP_CONSTANT):
    return constantInstruction("OP_CONSTANT", bytecodeSeq, offset);

  case (OP_NEGATE):
    return simpleInstruction("OP_NEGATE", offset);
  case OP_ADD:
    return simpleInstruction("OP_ADD", offset);
  case OP_SUBTRACT:
    return simpleInstruction("OP_SUBTRACT", offset);
  case OP_MULTIPLY:
    return simpleInstruction("OP_MULTIPLY", offset);
  case OP_DIVIDE:
    return simpleInstruction("OP_DIVIDE", offset);
  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
