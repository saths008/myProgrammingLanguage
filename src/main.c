#include "bytecodeSeq.h"
#include "debug.h"
#include "virtualMachine.h"
#include <stdlib.h>
static void repl(VirtualMachine *virtualMachine) {
  char line[1024];
  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    interpret(&virtualMachine, line);
  }
}
static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }
  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buffer = (char *)malloc(fileSize + 1);
  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }
  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}
static void runFile(VirtualMachine *virtualMachine, const char *path) {
  char *source = readFile(path);
  InterpretResultCode result = interpret(virtualMachine, source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR)
    exit(65);
  if (result == INTERPRET_RUNTIME_ERROR)
    exit(70);
}
int main(int argc, const char *argv[]) {
  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: %s [path]\n", argv[0]);
    exit(64);
  }
  VirtualMachine virtualMachine;
  initVirtualMachine(&virtualMachine);
  // interpret(&virtualMachine, &bytecodeSeq);
  freeVirtualMachine(&virtualMachine);
  return 0;
}
