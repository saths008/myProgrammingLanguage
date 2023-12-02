#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include <stdint.h>

typedef enum {
  OP_RETURN,
} OpCode;

typedef struct {
  uint8_t *code;
  int count;
  int capacity;
} Chunk;
void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte);
void freeChunk(Chunk *chunk);
#endif
