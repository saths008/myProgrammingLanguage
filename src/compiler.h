#ifndef c_compiler_h
#define c_compiler_h
#include "bytecodeSeq.h"
#include "scanner.h"
typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT, // =
  PREC_OR,         // or
  PREC_AND,        // and
  PREC_EQUALITY,   // == !=
  PREC_COMPARISON, // < > <= >=
  PREC_TERM,       // + -
  PREC_FACTOR,     // * /
  PREC_UNARY,      // ! -
  PREC_CALL,       // . ()
  PREC_PRIMARY
} Precedence;
typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;
void initParser();

// Populate bytecodeSeq with the bytecode translation of sourceCode
// Returns false if there is a compilation error
bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq);
static ParseRule *getRule(TokenType type);

static void expression();
static void literal();
static void parsePrecedence(Precedence precedence);
static void advanceToken();
static void errorAtCurrent(const char *message);
static void error(const char *message);
static void errorAt(Token *token, const char *message);
static void consume(TokenType type, const char *message);

#endif
