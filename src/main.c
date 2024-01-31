#include "debug.h"

int main(int argc, const char *argv[]) {
  BytecodeSeq bytecodeSeq;
  initBytecodeSeq(&bytecodeSeq);
  int constantIndexInConstantPool = addConstant(&bytecodeSeq, 5.6);
  writeBytecodeSeq(&bytecodeSeq, OP_CONSTANT, 257);
  writeBytecodeSeq(&bytecodeSeq, constantIndexInConstantPool, 257);
  writeBytecodeSeq(&bytecodeSeq, OP_RETURN, 258);
  disassembleBytecodeSeq(&bytecodeSeq, "BytecodeSeq");
  freeBytecodeSeq(&bytecodeSeq);
  return 0;
}
