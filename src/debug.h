#ifndef c_debug_h
#define c_debug_h

#include "bytecodeSeq.h"

void disassembleBytecodeSeq(BytecodeSeq *bytecodeSeq, const char *name);
int disassembleInstruction(BytecodeSeq *bytecodeSeq, int offset);

#endif
