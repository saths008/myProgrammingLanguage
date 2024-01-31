#include "bytecodeSeq.h"
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>
void initBytecodeSeq(BytecodeSeq *bytecodeSeq) {
  bytecodeSeq->capacity = 0;
  bytecodeSeq->count = 0;
  bytecodeSeq->code = NULL;
}

void writeBytecodeSeq(BytecodeSeq *bytecodeSeq, uint8_t byte) {
  if ((bytecodeSeq->count + 1) > bytecodeSeq->capacity) {

    int oldCapacity = bytecodeSeq->capacity;
    int newCapacity = CALCULATE_NEW_CAPACITY(oldCapacity);
    bytecodeSeq->code =
        GROW_ARRAY(uint8_t, bytecodeSeq->code, oldCapacity, newCapacity);
    bytecodeSeq->capacity = newCapacity;
  }
  bytecodeSeq->code[bytecodeSeq->count] = byte;
  bytecodeSeq->count++;
}

void freeBytecodeSeq(BytecodeSeq *bytecodeSeq) {
  FREE_ARRAY(uint8_t, bytecodeSeq->code, bytecodeSeq->capacity);
  // memory is left in an indeterminate state so initBytecodeSeq to reset
  // fields.
  initBytecodeSeq(bytecodeSeq);
}
