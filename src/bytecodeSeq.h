#ifndef c_bytecodeSeq_h
#define c_bytecodeSeq_h

#include "common.h"
#include "constantPoolArray.h"
#include <stdint.h>

typedef enum {
  OP_RETURN,
  OP_CONSTANT, // Load a constant from the constant pool with arg. of index
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_POP,
  OP_SET_GLOBAL,
  OP_EQUAL,
  OP_GET_GLOBAL,
  OP_DEFINE_GLOBAL,
  OP_GREATER,
  OP_LESS,
  OP_NEGATE,
  OP_PRINT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,

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
