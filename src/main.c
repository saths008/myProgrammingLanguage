#include "virtualMachine.h"
#include <stdio.h>
#include <stdlib.h>

static void repl() {
  char line[1024];
  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    printf("Interpreting...");
    interpret(line);
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

static void runFile(const char *path) {
  printf("Reading file...");
  char *source = readFile(path);
  printf("File read, interpreting...");

  InterpretResultCode result = interpret(source);
  printf("Interpreted file...");

  free(source);

  if (result == INTERPRET_COMPILE_ERROR)
    exit(65);
  if (result == INTERPRET_RUNTIME_ERROR)
    exit(70);
}

int main(int argc, const char *argv[]) {
  initVirtualMachine();
  printf("Virtual machine initialized");

  if (argc == 1) {
    printf("In repl");
    repl();
    freeVirtualMachine();
  } else if (argc == 2) {

    runFile(argv[1]);
    freeVirtualMachine();
  }
  if (argc > 2) {
    freeVirtualMachine();

    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  return 0;
}
