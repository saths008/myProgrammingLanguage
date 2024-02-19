#include "virtualMachine.h"
#include "bytecodeSeq.h"
#include "compiler.h"
#include "debug.h"
#include "value.h"
#include <stdarg.h>

static void resetStack(VirtualMachine *virtualMachine) {
  virtualMachine->stackTop = virtualMachine->stack;
}
void freeVirtualMachine(VirtualMachine *virtualMachine) {
  if (virtualMachine->bytecodeSeq != NULL) {
    freeBytecodeSeq(virtualMachine->bytecodeSeq);
  }
}

void initVirtualMachine(VirtualMachine *virtualMachine) {
  virtualMachine->bytecodeSeq = NULL;
  resetStack(virtualMachine);
}
void push(VirtualMachine *virtualMachine, Value value) {
  *virtualMachine->stackTop = value;
  virtualMachine->stackTop++;
}
Value pop(VirtualMachine *virtualMachine) {
  virtualMachine->stackTop--;
  return *virtualMachine->stackTop;
}

InterpretResultCode interpret(VirtualMachine *virtualMachine,
                              const char *sourceCode) {
  BytecodeSeq bytecodeSeq;
  initBytecodeSeq(&bytecodeSeq);
  if (!compile(sourceCode, &bytecodeSeq)) {
    freeBytecodeSeq(&bytecodeSeq);
    return INTERPRET_COMPILE_ERROR;
  } else {
    virtualMachine->bytecodeSeq = &bytecodeSeq;
    virtualMachine->instructionPointer = virtualMachine->bytecodeSeq->code;
    InterpretResultCode result = run(virtualMachine);
    freeBytecodeSeq(&bytecodeSeq);
    return result;
  }
}
static void runtimeError(VirtualMachine *vm, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm->instructionPointer - vm->bytecodeSeq->code - 1;
  int line = vm->bytecodeSeq->lineNumbers[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  resetStack(vm);
}
static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}
static Value peek(VirtualMachine *vm, int distance) {
  return vm->stackTop[-1 - distance];
}
InterpretResultCode run(VirtualMachine *virtualMachine) {

#define READ_BYTE() (*(virtualMachine->instructionPointer++))
#define READ_CONSTANT()                                                        \
  (virtualMachine->bytecodeSeq->constantPoolArray.values[READ_BYTE()])

#define BINARY_OP(valueType, op)                                               \
  do {                                                                         \
    if (!IS_NUMBER(peek(virtualMachine, 0)) ||                                 \
        !IS_NUMBER(peek(virtualMachine, 1))) {                                 \
      runtimeError(virtualMachine, "Operands must be numbers.");               \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
    double b = AS_NUMBER(pop(virtualMachine));                                 \
    double a = AS_NUMBER(pop(virtualMachine));                                 \
    push(virtualMachine, valueType(a op b));                                   \
  } while (false)

  while (true) {
// Flag to disassemble the instruction
#ifdef DEBUG_INSTR_EXECUTION
    printf("          ");
    for (Value *slot = virtualMachine->stack; slot < virtualMachine->stackTop;
         slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");

    disassembleInstruction(virtualMachine->bytecodeSeq,
                           (int)(virtualMachine->instructionPointer -
                                 virtualMachine->bytecodeSeq->code));
#endif
    uint8_t instruction = READ_BYTE();
    switch (instruction) {
    case OP_RETURN: {
      printValue(pop(virtualMachine));
      printf("\n");
      return INTERPRET_OK;
    }
    case OP_GREATER: {
      BINARY_OP(BOOL_VAL, >);
      break;
    }
    case OP_LESS: {
      BINARY_OP(BOOL_VAL, <);
      break;
    }
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      push(virtualMachine, constant);
      break;
    }
    case OP_NIL: {
      push(virtualMachine, NIL_VAL);
      break;
    }
    case OP_TRUE: {
      push(virtualMachine, BOOL_VAL(true));
      break;
    }
    case OP_NOT:
      push(virtualMachine, BOOL_VAL(isFalsey(pop(virtualMachine))));
      break;
    case OP_EQUAL: {
      Value b = pop(virtualMachine);
      Value a = pop(virtualMachine);
      push(virtualMachine, BOOL_VAL(valuesEqual(a, b)));
      break;
    }

    case OP_FALSE: {
      push(virtualMachine, BOOL_VAL(false));
      break;
    }
    case OP_NEGATE: {
      if (!IS_NUMBER(peek(virtualMachine, 0))) {
        runtimeError(virtualMachine, "Operand must be a number.");
        return INTERPRET_RUNTIME_ERROR;
      }
      push(virtualMachine, NUMBER_VAL(-AS_NUMBER(pop(virtualMachine))));
      break;
    }
    case OP_ADD: {
      BINARY_OP(NUMBER_VAL, +);

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
  }
}
