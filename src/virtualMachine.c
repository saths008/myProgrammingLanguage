#include "virtualMachine.h"
#include "bytecodeSeq.h"
#include "compiler.h"
#include "debug.h"
#include "hashTable.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <stdarg.h>
#include <string.h>
VirtualMachine virtualMachine;
static void resetStack() { virtualMachine.stackTop = virtualMachine.stack; }
void freeVirtualMachine() {
  freeHashTable(&virtualMachine.strings);
  freeHashTable(&virtualMachine.globals);
  freeObjects();
}

void initVirtualMachine() {
  virtualMachine.bytecodeSeq = NULL;
  resetStack();
  virtualMachine.objects = NULL;
  initHashTable(&virtualMachine.globals);

  initHashTable(&virtualMachine.strings);
}
void push(Value value) {
  *virtualMachine.stackTop = value;
  virtualMachine.stackTop++;
}
Value pop() {
  virtualMachine.stackTop--;
  return *virtualMachine.stackTop;
}

InterpretResultCode interpret(const char *sourceCode) {
  BytecodeSeq bytecodeSeq;
  initBytecodeSeq(&bytecodeSeq);
  if (!compile(sourceCode, &bytecodeSeq)) {
    freeBytecodeSeq(&bytecodeSeq);
    return INTERPRET_COMPILE_ERROR;
  } else {
    virtualMachine.bytecodeSeq = &bytecodeSeq;
    virtualMachine.instructionPointer = virtualMachine.bytecodeSeq->code;
    InterpretResultCode result = run();
    freeBytecodeSeq(&bytecodeSeq);
    return result;
  }
}
static void runtimeError(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction =
      virtualMachine.instructionPointer - virtualMachine.bytecodeSeq->code - 1;
  int line = virtualMachine.bytecodeSeq->lineNumbers[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack();
}
static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}
static Value peek(int distance) {
  return virtualMachine.stackTop[-1 - distance];
}
static void concatenate() {
  ObjString *b = AS_STRING(pop());
  ObjString *a = AS_STRING(pop());

  int length = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';

  ObjString *result = takeString(chars, length);
  push(OBJ_VAL(result));
}
InterpretResultCode run() {

#define READ_BYTE() (*(virtualMachine.instructionPointer++))
#define READ_CONSTANT()                                                        \
  (virtualMachine.bytecodeSeq->constantPoolArray.values[READ_BYTE()])
#define READ_SHORT()                                                           \
  (virtualMachine.instructionPointer += 2,                                     \
   (uint16_t)((virtualMachine.instructionPointer[-2] << 8) |                   \
              virtualMachine.instructionPointer[-1]))
#define READ_STRING() AS_STRING(READ_CONSTANT())

#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1))) {                          \
      runtimeError("Operands must be numbers.");                               \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop());                                               \
    double a = AS_NUMBER(pop());                                               \
    push(valueType(a op b));                                                   \
  } while (false)

  while (true) {
// Flag to disassemble the instruction
#ifdef DEBUG_INSTR_EXECUTION
    printf("          ");
    for (Value *slot = virtualMachine.stack; slot < virtualMachine.stackTop;
         slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");

    disassembleInstruction(virtualMachine.bytecodeSeq,
                           (int)(virtualMachine.instructionPointer -
                                 virtualMachine.bytecodeSeq->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_RETURN: {
      printValue(pop());
      printf("\n");
      return INTERPRET_OK;
    }
    case OP_LOOP: {
      uint16_t offset = READ_SHORT();
      virtualMachine.instructionPointer -= offset;
      break;
    }
    case OP_JUMP: {
      uint16_t offset = READ_SHORT();
      virtualMachine.instructionPointer += offset;
      break;
    }
    case OP_JUMP_IF_FALSE: {
      uint16_t offset = READ_SHORT();
      if (isFalsey(peek(0)))
        virtualMachine.instructionPointer += offset;
      break;
    }
    case OP_GREATER: {
      BINARY_OP(BOOL_VAL, >);
      break;
    }
    case OP_DEFINE_GLOBAL: {
      ObjString *name = READ_STRING();
      tableSet(&virtualMachine.globals, name, peek(0));
      pop();
      break;
    }
    case OP_POP: {
      pop();
      break;
    }
    case OP_GET_LOCAL: {
      uint8_t slot = READ_BYTE();
      push(virtualMachine.stack[slot]);
      break;
    }
    case OP_SET_LOCAL: {
      uint8_t slot = READ_BYTE();
      virtualMachine.stack[slot] = peek(0);
      break;
    }
    case OP_GET_GLOBAL: {
      ObjString *name = READ_STRING();
      Value value;
      if (!tableGet(&virtualMachine.globals, name, &value)) {
        runtimeError("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      push(value);
      break;
    }
    case OP_PRINT: {
      printValue(pop());
      printf("\n");
      break;
    }
    case OP_LESS: {
      BINARY_OP(BOOL_VAL, <);
      break;
    }
    case OP_ADD: {
      if (IS_STRING(peek(0)) && IS_STRING(peek(1))) {
        concatenate();
      } else if (IS_NUMBER(peek(0)) && IS_NUMBER(peek(1))) {
        double b = AS_NUMBER(pop());
        double a = AS_NUMBER(pop());
        push(NUMBER_VAL(a + b));
      } else {
        runtimeError("Operands must be two numbers or two strings.");
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(constant);
      break;
    }
    case OP_NIL: {
      push(NIL_VAL);
      break;
    }
    case OP_TRUE: {
      push(BOOL_VAL(true));
      break;
    }
    case OP_NOT:
      push(BOOL_VAL(isFalsey(pop())));
      break;
    case OP_SET_GLOBAL: {
      ObjString *name = READ_STRING();
      if (tableSet(&virtualMachine.globals, name, peek(0))) {
        tableDelete(&virtualMachine.globals, name);
        runtimeError("Undefined variable '%s'.", name->chars);
        return INTERPRET_RUNTIME_ERROR;
      }
      break;
    }
    case OP_EQUAL: {
      Value b = pop();
      Value a = pop();
      push(BOOL_VAL(valuesEqual(a, b)));
      break;
    }

    case OP_FALSE: {
      push(BOOL_VAL(false));
      break;
    }
    case OP_NEGATE: {
      if (!IS_NUMBER(peek(0))) {
        runtimeError("Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(NUMBER_VAL(-AS_NUMBER(pop())));
      break;
    }
    case OP_SUBTRACT: {
      BINARY_OP(NUMBER_VAL, -);
      break;
    }
    case OP_MULTIPLY: {
      BINARY_OP(NUMBER_VAL, *);
      break;
    }
    case OP_DIVIDE: {
      BINARY_OP(NUMBER_VAL, /);
      break;
    }
    }
#undef BINARY_OP
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_SHORT
#undef READ_STRING
  }
}
