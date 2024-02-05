#include "scanner.h"
#include "common.h"
#include <string.h>
void initScanner(Scanner *scanner, const char *sourceCode) {
  scanner->start = sourceCode;
  scanner->current = sourceCode;
  scanner->line = 1;
}
static bool isAtEnd(Scanner *scanner) { return *scanner->current == '\0'; }
static Token makeToken(Scanner *scanner, TokenType type) {
  Token token;
  token.type = type;
  token.start = scanner->start;
  token.length = (int)(scanner->current - scanner->start);
  token.line = scanner->line;
  return token;
}
static Token errorToken(Scanner *scanner, const char *message) {
  Token token;
  token.type = TOKEN_ERROR;
  token.start = message;
  token.length = (int)strlen(message);
  token.line = scanner->line;
  return token;
}
static char advance(Scanner *scanner) {
  scanner->current++;
  return scanner->current[-1];
}
static bool match(Scanner *scanner, char expected) {
  if (isAtEnd(scanner))
    return false;
  if (*scanner->current != expected)
    return false;
  scanner->current++;
  return true;
}
static char peek(Scanner *scanner) { return *scanner->current; }
static char peekNext(Scanner *scanner) {
  if (isAtEnd(scanner))
    return '\0';
  return scanner->current[1];
}
static void skipWhitespaceAndComments(Scanner *scanner) {
  for (;;) {
    char c = peek(scanner);
    switch (c) {
    case ' ':
    case '\r':
    case '\t':
      advance(scanner);
      break;
    case '/':
      if (peekNext(scanner) == '/') {
        while (peek(scanner) != '\n' && !isAtEnd(scanner))
          advance(scanner);
      } else {
        return;
      }
      break;
    default:
      return;
    }
  }
}
static Token string(Scanner *scanner) {
  while (peek(scanner) != '"' && !isAtEnd(scanner)) {
    if (peek(scanner) == '\n')
      scanner->line++;
    advance(scanner);
  }

  if (isAtEnd(scanner))
    return errorToken(scanner, "Unterminated string.");

  advance(scanner);
  return makeToken(scanner, TOKEN_STRING);
}
static bool isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
static TokenType identifierType() { return TOKEN_IDENTIFIER; }
static bool isDigit(char c) { return c >= '0' && c <= '9'; }

static Token identifier(Scanner *scanner) {
  while (isAlpha(peek(scanner)) || isDigit(peek(scanner)))
    advance(scanner);
  return makeToken(scanner, identifierType());
}
static Token number(Scanner *scanner) {
  while (isDigit(peek(scanner)))
    advance(scanner);

  if (peek(scanner) == '.' && isDigit(peekNext(scanner))) {
    advance(scanner);

    while (isDigit(peek(scanner)))
      advance(scanner);
  }

  return makeToken(scanner, TOKEN_NUMBER);
}

Token scanToken(Scanner *scanner) {
  skipWhitespaceAndComments(scanner);
  scanner->start = scanner->current;

  if (isAtEnd(scanner))
    return makeToken(scanner, TOKEN_EOF);
  char c = advance(scanner);
  if (isDigit(c))
    return number(scanner);
  if (isAlpha(c))
    return identifier();
  switch (c) {
  case '(':
    return makeToken(scanner, TOKEN_LEFT_PAREN);
  case ')':
    return makeToken(scanner, TOKEN_RIGHT_PAREN);
  case '{':
    return makeToken(scanner, TOKEN_LEFT_BRACE);
  case '}':
    return makeToken(scanner, TOKEN_RIGHT_BRACE);
  case ';':
    return makeToken(scanner, TOKEN_SEMICOLON);
  case ',':
    return makeToken(scanner, TOKEN_COMMA);
  case '.':
    return makeToken(scanner, TOKEN_DOT);
  case '-':
    return makeToken(scanner, TOKEN_MINUS);
  case '+':
    return makeToken(scanner, TOKEN_PLUS);
  case '/':
    return makeToken(scanner, TOKEN_SLASH);
  case '*':
    return makeToken(scanner, TOKEN_STAR);
  case '!':
    return makeToken(scanner,
                     match(scanner, '=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
  case '=':
    return makeToken(scanner,
                     match(scanner, '=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
  case '<':
    return makeToken(scanner,
                     match(scanner, '=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
  case '>':
    return makeToken(scanner,
                     match(scanner, '=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
  }
  return errorToken(scanner, "Unexpected character.");
}
