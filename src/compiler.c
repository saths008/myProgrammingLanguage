#include "compiler.h"
#include "bytecodeSeq.h"
#include "debug.h"
#include "object.h"
#include "scanner.h"
#include "value.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Parser parser;
Compiler *current = NULL;
BytecodeSeq *compilingBytecodeSeq;
static ParseRule *getRule(TokenType type);

static BytecodeSeq *currentBytecodeSeq() { return compilingBytecodeSeq; }
static void varDeclaration();
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
    error("Too many constants in one bytecodeSeq.");
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
static void initCompiler(Compiler *compiler) {
  compiler->localCount = 0;
  compiler->scopeDepth = 0;
  current = compiler;
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
static void beginScope() { current->scopeDepth++; }
static void endScope() {
  current->scopeDepth--;
  while (current->localCount > 0 &&
         current->locals[current->localCount - 1].depth > current->scopeDepth) {
    emitByte(OP_POP);
    current->localCount--;
  }
}
static void expression();
static void statement();
static void declaration();
// static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);
static uint8_t identifierConstant(Token *name) {
  return makeConstant(OBJ_VAL(copyString(name->start, name->length)));
}
static void addLocal(Token name) {
  if (current->localCount == UINT8_COUNT) {
    error("Too many local variables in function.");
    return;
  }
  Local *local = &current->locals[current->localCount++];
  local->name = name;
  local->depth = -1;
}
static bool identifiersEqual(Token *a, Token *b) {
  if (a->length != b->length)
    return false;
  return memcmp(a->start, b->start, a->length) == 0;
}
static int resolveLocal(Compiler *compiler, Token *name) {
  for (int i = compiler->localCount - 1; i >= 0; i--) {
    Local *local = &compiler->locals[i];
    if (identifiersEqual(name, &local->name)) {
      if (local->depth == -1) {
        error("Can't read local variable in its own initializer.");
      }
      return i;
    }
  }

  return -1;
}
static void declareVariable() {
  if (current->scopeDepth == 0)
    return;

  Token *name = &parser.previous;
  for (int i = current->localCount - 1; i >= 0; i--) {
    Local *local = &current->locals[i];
    if (local->depth != -1 && local->depth < current->scopeDepth) {
      break;
    }

    if (identifiersEqual(name, &local->name)) {
      error("Already a variable with this name in this scope.");
    }
  }
  addLocal(*name);
}
static void markInitialised() {
  current->locals[current->localCount - 1].depth = current->scopeDepth;
}
static void defineVariable(uint8_t global) {
  if (current->scopeDepth > 0) {
    markInitialised();
    return;
  }
  emitTwoBytes(OP_DEFINE_GLOBAL, global);
}

static void expressionStatement() {
  expression();
  consume(TOKEN_SEMICOLON, "Expect ';' after expression.");
  emitByte(OP_POP);
}
static void block() {
  while (!check(TOKEN_RIGHT_BRACE) && !check(TOKEN_EOF)) {
    declaration();
  }

  consume(TOKEN_RIGHT_BRACE, "Expect '}' after block.");
}
static void patchJump(int offset) {
  int jump = currentBytecodeSeq()->count - offset - 2;

  if (jump > UINT16_MAX) {
    error("Too much code to jump over.");
  }

  currentBytecodeSeq()->code[offset] = (jump >> 8) & 0xff;
  currentBytecodeSeq()->code[offset + 1] = jump & 0xff;
}
static int emitJump(uint8_t instruction) {
  emitByte(instruction);
  emitByte(0xff);
  emitByte(0xff);
  return currentBytecodeSeq()->count - 2;
}
static void ifStatement() {
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'if'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

  int thenJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();
  int elseJump = emitJump(OP_JUMP);
  patchJump(thenJump);
  emitByte(OP_POP);
  if (match(TOKEN_ELSE)) {
    statement();
  }
  patchJump(elseJump);
}
static void emitLoop(int loopStart) {
  emitByte(OP_LOOP);

  int offset = currentBytecodeSeq()->count - loopStart + 2;
  if (offset > UINT16_MAX)
    error("Loop body too large.");

  emitByte((offset >> 8) & 0xff);
  emitByte(offset & 0xff);
}
static void whileStatement() {
  int loopStart = currentBytecodeSeq()->count;
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'while'.");
  expression();
  consume(TOKEN_RIGHT_PAREN, "Expect ')' after condition.");

  int exitJump = emitJump(OP_JUMP_IF_FALSE);
  emitByte(OP_POP);
  statement();
  emitLoop(loopStart);
  patchJump(exitJump);
  emitByte(OP_POP);
}
static void forStatement() {
  beginScope();
  consume(TOKEN_LEFT_PAREN, "Expect '(' after 'for'.");
  consume(TOKEN_SEMICOLON, "Expect ';'.");
  if (match(TOKEN_SEMICOLON)) {
    // No initializer.
  } else if (match(TOKEN_VAR)) {
    varDeclaration();
  } else {
    expressionStatement();
  }
  int loopStart = currentBytecodeSeq()->count;
  int exitJump = -1;
  if (!match(TOKEN_SEMICOLON)) {
    expression();
    consume(TOKEN_SEMICOLON, "Expect ';' after loop condition.");

    exitJump = emitJump(OP_JUMP_IF_FALSE);
    emitByte(OP_POP); // Condition.
  }
  if (!match(TOKEN_RIGHT_PAREN)) {
    int bodyJump = emitJump(OP_JUMP);
    int incrementStart = currentBytecodeSeq()->count;
    expression();
    emitByte(OP_POP);
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after for clauses.");

    emitLoop(loopStart);
    loopStart = incrementStart;
    patchJump(bodyJump);
  }

  statement();
  emitLoop(loopStart);
  if (exitJump != -1) {
    patchJump(exitJump);
    emitByte(OP_POP); // Condition.
  }
  endScope();
}
static void statement() {
  if (match(TOKEN_PRINT)) {
    printStatement();
  } else if (match(TOKEN_IF)) {
    ifStatement();
  } else if (match(TOKEN_FOR)) {
    forStatement();
  } else if (match(TOKEN_WHILE)) {
    whileStatement();
  } else if (match(TOKEN_LEFT_BRACE)) {
    beginScope();
    block();
    endScope();
  } else {
    expressionStatement();
  }
}
static uint8_t parseVariable(const char *errorMessage) {
  consume(TOKEN_IDENTIFIER, errorMessage);
  declareVariable();
  if (current->scopeDepth > 0)
    return 0;
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
  Compiler compiler;
  initCompiler(&compiler);
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
  uint8_t getOp, setOp;
  int arg = resolveLocal(current, &name);
  if (arg != -1) {
    getOp = OP_GET_LOCAL;
    setOp = OP_SET_LOCAL;
  } else {
    arg = identifierConstant(&name);
    getOp = OP_GET_GLOBAL;
    setOp = OP_SET_GLOBAL;
  }
  if (canAssign && match(TOKEN_EQUAL)) {
    expression();
    emitTwoBytes(setOp, (uint8_t)arg);
  } else {
    emitTwoBytes(getOp, (uint8_t)arg);
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
static void and_(bool canAssign) {
  int endJump = emitJump(OP_JUMP_IF_FALSE);

  emitByte(OP_POP);
  parsePrecedence(PREC_AND);

  patchJump(endJump);
}
static void or_(bool canAssign) {
  int elseJump = emitJump(OP_JUMP_IF_FALSE);
  int endJump = emitJump(OP_JUMP);

  patchJump(elseJump);
  emitByte(OP_POP);

  parsePrecedence(PREC_OR);
  patchJump(endJump);
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
    [TOKEN_AND] = {NULL, and_, PREC_AND},
    [TOKEN_CLASS] = {NULL, NULL, PREC_NONE},
    [TOKEN_ELSE] = {NULL, NULL, PREC_NONE},
    [TOKEN_FALSE] = {literal, NULL, PREC_NONE},
    [TOKEN_FOR] = {NULL, NULL, PREC_NONE},
    [TOKEN_FUN] = {NULL, NULL, PREC_NONE},
    [TOKEN_IF] = {NULL, NULL, PREC_NONE},
    [TOKEN_NIL] = {literal, NULL, PREC_NONE},
    [TOKEN_OR] = {NULL, or_, PREC_OR},
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
