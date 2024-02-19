#include "compiler.h"
#include "bytecodeSeq.h"
#include "debug.h"
#include "scanner.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

Scanner scanner;
Parser parser;
BytecodeSeq *compilingBytecodeSeq;
static ParseRule *getRule(TokenType type);

static BytecodeSeq *currentBytecodeSeq() { return compilingBytecodeSeq; }

static void emitByte(uint8_t byte) {
  writeBytecodeSeq(currentBytecodeSeq(), byte, parser.previous.line);
}
static void consume(TokenType type, const char *message) {
  if (parser.current.type == type) {
    advanceToken();
  } else {
    errorAtCurrent(message);
  }
}
static void advanceToken() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanToken(&scanner);
    if (parser.current.type != TOKEN_ERROR) {
      printf("Token type: %d\n", parser.current.type);
      break;
    } else {
      printf("Error token type: %d\n", parser.current.type);
      errorAtCurrent(parser.current.start);
    }
  }
}
static void errorAtCurrent(const char *message) {
  errorAt(&parser.current, message);
}
static void error(const char *message) { errorAt(&parser.previous, message); }

static void errorAt(Token *token, const char *message) {
  if (parser.panicMode)
    return;
  parser.panicMode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser.hadError = true;
}
void initParser() {
  parser.hadError = false;
  parser.panicMode = false;
}
static void emitReturn() { emitByte(OP_RETURN); }

static uint8_t makeConstant(Value value) {
  int constant = addConstant(currentBytecodeSeq(), value);
  if (constant > UINT8_MAX) {
    error("Too many constants in one chunk.");
    return 0;
  } else {
    return (uint8_t)constant;
  }
}

static void emitTwoBytes(uint8_t byte1, uint8_t byte2) {
  emitByte(byte1);
  emitByte(byte2);
}
static void emitConstant(Value value) {
  emitTwoBytes(OP_CONSTANT, makeConstant(value));
}

void endCompiler() {
  emitByte(OP_RETURN);
#ifdef DEBUG_PRINT_CODE
  if (!parser.hadError) {
    disassembleBytecodeSeq(currentBytecodeSeq(), "code");
  }
#endif
  emitReturn();
}
static void expression();
// static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq) {
  printf("Source code:\n%s\n", sourceCode);
  initScanner(&scanner, sourceCode);
  compilingBytecodeSeq = bytecodeSeq;
  initParser();
  advanceToken();
  expression();
  consume(TOKEN_EOF, "Expect end of expression.");
  endCompiler();
  return !parser.hadError;
}
static void parsePrecedence(Precedence precedence) {
  advanceToken();
  ParseFn prefixRule = getRule(parser.previous.type)->prefix;
  if (prefixRule == NULL) {
    error("Expect expression.");
    return;
  }

  prefixRule();
  while (precedence <= getRule(parser.current.type)->precedence) {
    advanceToken();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule();
  }
}

// Parse all precedence levels >= PREC_ASSIGNMENT
static void expression() {
  printf("Expression\n");
  parsePrecedence(PREC_ASSIGNMENT);
}
static void number() {
  printf("Number\n");
  if (parser.previous.start == NULL) {
    printf("Parser->previous.start is null");
    return;
  }
  double value = strtod(parser.previous.start, NULL);
  printf("Value: %f\n", value);
  emitConstant(NUMBER_VAL(value));
}
static void binary() {
  printf("Binary\n");
  TokenType operatorType = parser.previous.type;
  ParseRule *rule = getRule(operatorType);
  parsePrecedence((Precedence)(rule->precedence + 1));

  switch (operatorType) {
  case TOKEN_BANG_EQUAL: {
    emitTwoBytes(OP_EQUAL, OP_NOT);
    break;
  }
  case TOKEN_EQUAL_EQUAL: {
    emitByte(OP_EQUAL);
    break;
  }
  case TOKEN_GREATER: {
    emitByte(OP_GREATER);
    break;
  }
  case TOKEN_GREATER_EQUAL: {
    emitTwoBytes(OP_LESS, OP_NOT);
    break;
  }
  case TOKEN_LESS: {
    emitByte(OP_LESS);
    break;
  }
  case TOKEN_LESS_EQUAL: {
    emitTwoBytes(OP_GREATER, OP_NOT);
    break;
  }

  case TOKEN_PLUS: {
    emitByte(OP_ADD);
    break;
  }
  case TOKEN_MINUS: {
    emitByte(OP_SUBTRACT);
    break;
  }
  case TOKEN_STAR:
    emitByte(OP_MULTIPLY);
    break;
  case TOKEN_SLASH:
    emitByte(OP_DIVIDE);
    break;
  default:
    return; // Unreachable.
  }
}

static void grouping() {
  printf("Grouping\n");
  // Assume that ( has already been consumed
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}
static void literal() {
  switch (parser.previous.type) {
  case TOKEN_FALSE:
    emitByte(OP_FALSE);
    break;
  case TOKEN_NIL:
    emitByte(OP_NIL);
    break;
  case TOKEN_TRUE:
    emitByte(OP_TRUE);
    break;
  default:
    return; // Unreachable.
  }
}

static void unary() {
  printf("Unary\n");
  TokenType operatorType = parser.previous.type;

  parsePrecedence(PREC_UNARY);

  switch (operatorType) {
  case TOKEN_BANG:
    emitByte(OP_NOT);
    break;
  case TOKEN_MINUS:
    // Push the operand and then the negate instruction
    emitByte(OP_NEGATE);
    break;
  default:
    return;
  }
}
ParseRule rules[] = {
    [TOKEN_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOKEN_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOKEN_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOKEN_COMMA] = {NULL, NULL, PREC_NONE},
    [TOKEN_DOT] = {NULL, NULL, PREC_NONE},
    [TOKEN_MINUS] = {unary, binary, PREC_TERM},
    [TOKEN_PLUS] = {NULL, binary, PREC_TERM},
    [TOKEN_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOKEN_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOKEN_STAR] = {NULL, binary, PREC_FACTOR},
    [TOKEN_BANG] = {unary, NULL, PREC_NONE},
    [TOKEN_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOKEN_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOKEN_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOKEN_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOKEN_STRING] = {NULL, NULL, PREC_NONE},
    [TOKEN_NUMBER] = {number, NULL, PREC_NONE},
    [TOKEN_AND] = {NULL, NULL, PREC_NONE},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, NULL, PREC_NONE},
    [TOKEN_PRINT] = {NULL, NULL, PREC_NONE},
    [TOKEN_RETURN] = {NULL, NULL, PREC_NONE},
    [TOKEN_SUPER] = {NULL, NULL, PREC_NONE},
    [TOKEN_THIS] = {NULL, NULL, PREC_NONE},
    [TOKEN_TRUE] = {literal, NULL, PREC_NONE},
    [TOKEN_VAR] = {NULL, NULL, PREC_NONE},
    [TOKEN_WHILE] = {NULL, NULL, PREC_NONE},
    [TOKEN_ERROR] = {NULL, NULL, PREC_NONE},
    [TOKEN_EOF] = {NULL, NULL, PREC_NONE},
};

static ParseRule *getRule(TokenType type) { return &rules[type]; }
