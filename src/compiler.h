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
typedef struct {
  Token current;
  Token previous;
  bool hadError;
  bool panicMode;
} Parser;
void initParser(Parser *parser);

// Populate bytecodeSeq with the bytecode translation of sourceCode
// Returns false if there is a compilation error
bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq);
static void expression();
static void parsePrecedence(Precedence precedence);
static void advanceToken(Scanner *scanner, Parser *parser);
static void errorAtCurrent(Parser *parser, const char *message);
static void error(Parser *parser, const char *message);
static void errorAt(Parser *parser, Token *token, const char *message);
static void consume(Scanner *scanner, Parser *parser, TokenType type,
                    const char *message);

#endif
