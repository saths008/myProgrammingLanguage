#include "bytecodeSeq.h"
#include "constantPoolArray.h"
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>

void initBytecodeSeq(BytecodeSeq *bytecodeSeq) {
  bytecodeSeq->capacity = 0;
  bytecodeSeq->count = 0;
  bytecodeSeq->code = NULL;
  bytecodeSeq->lineNumbers = NULL;
  initConstantPoolArray(&bytecodeSeq->constantPoolArray);
}

void writeBytecodeSeq(BytecodeSeq *bytecodeSeq, uint8_t byte, int lineNumber) {
  if ((bytecodeSeq->count + 1) > bytecodeSeq->capacity) {

    int oldCapacity = bytecodeSeq->capacity;
    int newCapacity = CALCULATE_NEW_CAPACITY(oldCapacity);
    bytecodeSeq->code =
        GROW_ARRAY(uint8_t, bytecodeSeq->code, oldCapacity, newCapacity);
    bytecodeSeq->lineNumbers =
        GROW_ARRAY(int, bytecodeSeq->lineNumbers, oldCapacity, newCapacity);
    bytecodeSeq->capacity = newCapacity;
  }
  bytecodeSeq->code[bytecodeSeq->count] = byte;
  bytecodeSeq->lineNumbers[bytecodeSeq->count] = lineNumber;
  bytecodeSeq->count++;
}

void freeBytecodeSeq(BytecodeSeq *bytecodeSeq) {
  FREE_ARRAY(int, bytecodeSeq->lineNumbers, bytecodeSeq->capacity);
  FREE_ARRAY(uint8_t, bytecodeSeq->code, bytecodeSeq->capacity);

  freeConstantPoolArray(&bytecodeSeq->constantPoolArray);
  // memory is left in an indeterminate state so initBytecodeSeq to reset
  // fields.
  initBytecodeSeq(bytecodeSeq);
}
int addConstant(BytecodeSeq *bytecodeSeq, Value value) {
  writeConstantPoolArray(&bytecodeSeq->constantPoolArray, value);
  return bytecodeSeq->constantPoolArray.count - 1;
}
