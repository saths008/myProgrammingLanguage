#include "compiler.h"
#include "bytecodeSeq.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>

Parser parser;
BytecodeSeq *compilingBytecodeSeq;
static ParseRule *getRule(TokenType type);

static BytecodeSeq *currentBytecodeSeq() { return compilingBytecodeSeq; }

static void emitByte(uint8_t byte) {
  writeBytecodeSeq(currentBytecodeSeq(), byte, parser.previous.line);
}
static bool check(TokenType type) { return parser.current.type == type; }

static void consume(TokenType type, const char *message) {
  if (parser.current.type == type) {
    advanceToken();
  } else {
    errorAtCurrent(message);
  }
}
static bool match(TokenType type) {
  if (!check(type))
    return false;
  advanceToken();
  return true;
}
static void printStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after value.");
  emitByte(OP_PRINT);
}
static void synchronise() {
  parser.panicMode = false;

  while (parser.current.type != TOKEN_EOF) {
    if (parser.previous.type == TOKEN_SEMICOLON)
      return;
    switch (parser.current.type) {
    case TOKEN_CLASS:
    case TOKEN_FUN:
    case TOKEN_VAR:
    case TOKEN_FOR:
    case TOKEN_IF:
    case TOKEN_WHILE:
    case TOKEN_PRINT:
    case TOKEN_RETURN:
      return;

    default:;
    }

    advanceToken();
  }
}
static void advanceToken() {
  parser.previous = parser.current;

  for (;;) {
    parser.current = scanToken();
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
static void statement();
static void declaration();
// static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static uint8_t identifierConstant(Token *name) {
  return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}
static void defineVariable(uint8_t global) {
  emitTwoBytes(OP_DEFINE_GLOBAL, global);
}
static void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
}
static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else {
    expressionStatement();
  }
}
static uint8_t parseVariable(const char *errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  return identifierConstant(&parser.previous);
}
static void varDeclaration() {
  uint8_t global = parseVariable("Expect variable name.");

  if (match(TOKEN_EQUAL)) {
    expression();
  } else {
    emitByte(OP_NIL);
  }
  consume(TOKEN_SEMICOLON, "Expect ';' after variable declaration.");

  defineVariable(global);
}
static void declaration() {
  if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    statement();
  }
  if (parser.panicMode) {
    synchronise();
  }
}
bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq) {
  printf("Source code:\n%s\n", sourceCode);
  initScanner(sourceCode);
  compilingBytecodeSeq = bytecodeSeq;
  initParser();
  advanceToken();

  while (!match(TOKEN_EOF)) {
    declaration();
  }
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

  bool canAssign = precedence <= PREC_ASSIGNMENT;
  prefixRule(canAssign);

  while (precedence <= getRule(parser.current.type)->precedence) {
    advanceToken();
    ParseFn infixRule = getRule(parser.previous.type)->infix;
    infixRule(canAssign);
  }
  if (canAssign && match(TOKEN_EQUAL)) {
    error("Invalid assignment target.");
  }
}

// Parse all precedence levels >= PREC_ASSIGNMENT
static void expression() {
  printf("Expression\n");
  parsePrecedence(PREC_ASSIGNMENT);
}
static void number(bool canAssign) {
  printf("Number\n");
  if (parser.previous.start == NULL) {
    printf("Parser->previous.start is null");
    return;
  }
  double value = strtod(parser.previous.start, NULL);
  printf("Value: %f\n", value);
  emitConstant(NUMBER_VAL(value));
}
static void binary(bool canAssign) {
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

static void grouping(bool canAssign) {
  printf("Grouping\n");
  // Assume that ( has already been consumed
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}
static void literal(bool canAssign) {
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
static void string(bool canAssign) {
  emitConstant(OBJ_VAL(
      copyString(parser.previous.start + 1, parser.previous.length - 2)));
}
static void namedVariable(Token name, bool canAssign) {
  uint8_t arg = identifierConstant(&name);
  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitTwoBytes(OP_SET_GLOBAL, arg);
  } else {
    emitTwoBytes(OP_GET_GLOBAL, arg);
  }
}
static void variable(bool canAssign) {
  namedVariable(parser.previous, canAssign);
}
static void unary(bool canAssign) {
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
    [TOKEN_IDENTIFIER] = {variable, NULL, PREC_NONE},
    [TOKEN_STRING] = {string, NULL, PREC_NONE},
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
