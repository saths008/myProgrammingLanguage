#ifndef c_bytecodeSeq_h
#define c_bytecodeSeq_h

#include "common.h"
#include "constantPoolArray.h"
#include <stdint.h>

typedef enum {
  OP_RETURN,
  OP_CONSTANT, // Load a constant from the constant pool with arg. of index
} OpCode;

typedef struct {
  uint8_t *code;
  int count;
  int capacity;
  int *lineNumbers;
  ConstantPoolArray constantPoolArray;
} BytecodeSeq;

void initBytecodeSeq(BytecodeSeq *bytecodeSeq);
void writeBytecodeSeq(BytecodeSeq *bytecodeSeq, uint8_t byte, int lineNumber);
void freeBytecodeSeq(BytecodeSeq *bytecodeSeq);
int addConstant(BytecodeSeq *bytecodeSeq, Value value);
#endif
