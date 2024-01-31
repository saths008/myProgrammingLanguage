#ifndef c_bytecodeSeq_h
#define c_bytecodeSeq_h

#include "common.h"
#include <stdint.h>

typedef enum {
  OP_RETURN,
} OpCode;

typedef struct {
  uint8_t *code;
  int count;
  int capacity;
} BytecodeSeq;
void initBytecodeSeq(BytecodeSeq *bytecodeSeq);
void writeBytecodeSeq(BytecodeSeq *bytecodeSeq, uint8_t byte);
void freeBytecodeSeq(BytecodeSeq *bytecodeSeq);
#endif
