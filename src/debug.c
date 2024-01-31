#include "debug.h"
#include <stdint.h>
#include <stdio.h>
static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

void disassembleBytecodeSeq(BytecodeSeq *bytecodeSeq, const char *name) {

  printf("== %s ==\n", name);
  for (int offset = 0; offset < bytecodeSeq->count;) {
    offset = disassembleInstruction(bytecodeSeq, offset);
  }
}
int disassembleInstruction(BytecodeSeq *bytecodeSeq, int offset) {
  printf("%04d ", offset);
  uint8_t instruction = bytecodeSeq->code[offset];
  switch (instruction) {
  case (OP_RETURN):
    return simpleInstruction("OP_RETURN", offset);

  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
