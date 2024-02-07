#include "compiler.h"
#include "scanner.h"
#include <stdio.h>

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
// bool compile(const char *sourceCode, BytecodeSeq *bytecodeSeq) {
//   Scanner scanner;
//   initScanner(&scanner, sourceCode);
//   Parser parser;
//   initParser(&parser);
//   advanceToken(&scanner, &parser);
//   expression();
//   consume(&scanner, &parser, TOKEN_EOF, "Expect end of expression.");
//   return !parser.hadError;
// }
