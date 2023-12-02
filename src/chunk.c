#include "chunk.h"
#include "memory.h"
#include <stdint.h>
#include <stdlib.h>
void initChunk(Chunk *chunk) {
  chunk->capacity = 0;
  chunk->count = 0;
  chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  if ((chunk->count + 1) > chunk->capacity) {

    int oldCapacity = chunk->capacity;
    int newCapacity = CALCULATE_NEW_CAPACITY(oldCapacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, newCapacity);
    chunk->capacity = newCapacity;
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  // memory is left in an indeterminate state so initChunk to reset fields.
  initChunk(chunk);
}
