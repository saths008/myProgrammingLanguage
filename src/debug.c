#include "debug.h"
#include "bytecodeSeq.h"
#include "object.h"
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
void printValue(Value value) {
  switch (value.type) {
  case VAL_BOOL:
    printf(AS_BOOL(value) ? "true" : "false");
    break;
  case VAL_NIL:
    printf("nil");
    break;
  case VAL_NUMBER:
    printf("%g", AS_NUMBER(value));
    break;
  case VAL_OBJ:
    printObject(value);
    break;
  }
}
void disassembleBytecodeSeq(BytecodeSeq *bytecodeSeq, const char *name) {

  printf("== %s ==\n", name);
  for (int offset = 0; offset < bytecodeSeq->count;) {
    offset = disassembleInstruction(bytecodeSeq, offset);
  }
}
static int byteInstruction(const char *name, BytecodeSeq *bytecodeSeq,
                           int offset) {
  uint8_t slot = bytecodeSeq->code[offset + 1];
  printf("%-16s %4d\n", name, slot);
  return offset + 2;
}
int disassembleInstruction(BytecodeSeq *bytecodeSeq, int offset) {
  printf("%04d ", offset);
  if (offset > 0 && bytecodeSeq->lineNumbers[offset] ==
                        bytecodeSeq->lineNumbers[offset - 1]) {
    printf("   \u2191 ");
  } else {
    printf("%4d ", bytecodeSeq->lineNumbers[offset]);
  }
  uint8_t instruction = bytecodeSeq->code[offset];
  switch (instruction) {
  case (OP_RETURN):
    return simpleInstruction("OP_RETURN", offset);
  case OP_POP:
    return simpleInstruction("OP_POP", offset);
  case OP_GET_LOCAL:
    return byteInstruction("OP_GET_LOCAL", bytecodeSeq, offset);
  case OP_SET_LOCAL:
    return byteInstruction("OP_SET_LOCAL", bytecodeSeq, offset);
  case OP_GET_GLOBAL:
    return constantInstruction("OP_GET_GLOBAL", bytecodeSeq, offset);
  case OP_DEFINE_GLOBAL:
    return constantInstruction("OP_DEFINE_GLOBAL", bytecodeSeq, offset);
  case OP_PRINT:
    return simpleInstruction("OP_PRINT", offset);
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
  case OP_NIL:
    return simpleInstruction("OP_NIL", offset);
  case OP_NOT:
    return simpleInstruction("OP_NOT", offset);
  case OP_TRUE:
    return simpleInstruction("OP_TRUE", offset);
  case OP_FALSE:
    return simpleInstruction("OP_FALSE", offset);
  case OP_SET_GLOBAL:
    return constantInstruction("OP_SET_GLOBAL", bytecodeSeq, offset);
  case OP_EQUAL:
    return simpleInstruction("OP_EQUAL", offset);
  case OP_GREATER:
    return simpleInstruction("OP_GREATER", offset);
  case OP_LESS:
    return simpleInstruction("OP_LESS", offset);

  default:
    printf("Unknown opcode %d\n", instruction);
    return offset + 1;
  }
}
