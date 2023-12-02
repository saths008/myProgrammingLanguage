#include "chunk.h"
#include "debug.h"
int main(int argc, const char *argv[]) {
  Chunk chunk;
  initChunk(&chunk);
  writeChunk(&chunk, OP_RETURN);
  disassembleChunk(&chunk, "Chunk 1");
  freeChunk(&chunk);
  return 0;
}
