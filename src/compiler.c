#include "compiler.h"
#include "bytecodeSeq.h"
#include "scanner.h"
#include <stdio.h>
#include <stdlib.h>

BytecodeSeq *compilingBytecodeSeq;

static BytecodeSeq *currentBytecodeSeq() { return compilingBytecodeSeq; }

static void emitByte(Parser *parser, uint8_t byte) {
  writeBytecodeSeq(currentBytecodeSeq(), byte, parser->previous.line);
}
static void consume(Scanner *scanner, Parser *parser, TokenType type,
                    const char *message) {
  if (parser->current.type == type) {
    advanceToken(scanner, parser);
  } else {
    errorAtCurrent(parser, message);
  }
}
static void advanceToken(Scanner *scanner, Parser *parser) {
  parser->previous = parser->current;

  for (;;) {
    parser->current = scanToken(scanner);
    if (parser->current.type != TOKEN_ERROR) {
      break;
    } else {
      errorAtCurrent(parser, parser->current.start);
    }
  }
}
static void errorAtCurrent(Parser *parser, const char *message) {
  errorAt(parser, &parser->current, message);
}
static void error(Parser *parser, const char *message) {
  errorAt(parser, &parser->previous, message);
}

static void errorAt(Parser *parser, Token *token, const char *message) {
  if (parser->panicMode)
    return;
  parser->panicMode = true;
  fprintf(stderr, "[line %d] Error", token->line);

  if (token->type == TOKEN_EOF) {
    fprintf(stderr, " at end");
  } else if (token->type == TOKEN_ERROR) {
    // Nothing.
  } else {
    fprintf(stderr, " at '%.*s'", token->length, token->start);
  }

  fprintf(stderr, ": %s\n", message);
  parser->hadError = true;
}
void initParser(Parser *parser) {
  parser->hadError = false;
  parser->panicMode = false;
}
static void emitReturn(Parser *parser) { emitByte(parser, OP_RETURN); }

static uint8_t makeConstant(Parser *parser, Value value) {
  int constant = addConstant(currentBytecodeSeq(), value);
  if (constant > UINT8_MAX) {
    error(parser, "Too many constants in one chunk.");
    return 0;
  } else {
    return (uint8_t)constant;
  }
}

static void emitTwoBytes(Parser *parser, uint8_t byte1, uint8_t byte2) {
  emitByte(parser, byte1);
  emitByte(parser, byte2);
}
static void emitConstant(Parser *parser, Value value) {
  emitTwoBytes(parser, OP_CONSTANT, makeConstant(parser, value));
}

void endCompiler(Parser *parser) { emitByte(parser, OP_RETURN); }
static void expression();
// static ParseRule *getRule(TokenType type);
static void parsePrecedence(Precedence precedence);

bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq) {
  Scanner scanner;
  initScanner(&scanner, sourceCode);
  compilingBytecodeSeq = bytecodeSeq;
  Parser parser;
  initParser(&parser);
  advanceToken(&scanner, &parser);
  expression();
  consume(&scanner, &parser, TOKEN_EOF, "Expect end of expression.");
  endCompiler(&parser);
  return !parser.hadError;
}
static void parsePrecedence(Precedence precedence) {}

// Parse all precedence levels >= PREC_ASSIGNMENT
static void expression() { parsePrecedence(PREC_ASSIGNMENT); }
static void number(Parser *parser) {
  double value = strtod(parser->previous.start, NULL);
  emitConstant(parser, value);
}

static void grouping(Scanner *scanner, Parser *parser) {
  // Assume that ( has already been consumed
  expression();
  consume(scanner, parser, TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}
static void unary(Parser *parser) {
  TokenType operatorType = parser->previous.type;

  parsePrecedence(PREC_UNARY);

  switch (operatorType) {
  case TOKEN_MINUS:
    // Push the operand and then the negate instruction
    emitByte(parser, OP_NEGATE);
    break;
  default:
    return;
  }
}
