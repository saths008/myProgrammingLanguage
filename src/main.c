#include "debug.h"
int main(int argc, const char *argv[]) {
  BytecodeSeq bytecodeSeq;
  initBytecodeSeq(&bytecodeSeq);
  writeBytecodeSeq(&bytecodeSeq, OP_RETURN);
  disassembleBytecodeSeq(&bytecodeSeq, "bytecodeSeq 1");
  freeBytecodeSeq(&bytecodeSeq);
  return 0;
}
