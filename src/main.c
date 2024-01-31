#include "bytecodeSeq.h"
#include "debug.h"
#include "virtualMachine.h"
int main(int argc, const char *argv[]) {
  BytecodeSeq bytecodeSeq;
  initBytecodeSeq(&bytecodeSeq);
  int constantIndexInConstantPool = addConstant(&bytecodeSeq, 1.2);
  writeBytecodeSeq(&bytecodeSeq, OP_CONSTANT, 1);
  writeBytecodeSeq(&bytecodeSeq, constantIndexInConstantPool, 1);

  int constantIndexInConstantPool2 = addConstant(&bytecodeSeq, 3.4);
  writeBytecodeSeq(&bytecodeSeq, OP_CONSTANT, 1);
  writeBytecodeSeq(&bytecodeSeq, constantIndexInConstantPool2, 1);

  writeBytecodeSeq(&bytecodeSeq, OP_ADD, 1);

  int constantIndexInConstantPool3 = addConstant(&bytecodeSeq, 5.6);
  writeBytecodeSeq(&bytecodeSeq, OP_CONSTANT, 1);
  writeBytecodeSeq(&bytecodeSeq, constantIndexInConstantPool3, 1);

  writeBytecodeSeq(&bytecodeSeq, OP_DIVIDE, 1);
  writeBytecodeSeq(&bytecodeSeq, OP_NEGATE, 1);
  writeBytecodeSeq(&bytecodeSeq, OP_RETURN, 1);

  // disassembleBytecodeSeq(&bytecodeSeq, "BytecodeSeq");
  VirtualMachine virtualMachine;
  initVirtualMachine(&virtualMachine);
  interpret(&virtualMachine, &bytecodeSeq);
  freeBytecodeSeq(&bytecodeSeq);
  return 0;
}
