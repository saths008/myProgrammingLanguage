#ifndef PARSER_HPP
#define PARSER_HPP
#include "expr.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>
class ParseError : public std::runtime_error {
public:
  ParseError(const std::string &message) : std::runtime_error(message) {}
};
class Parser {
private:
  std::shared_ptr<std::vector<std::unique_ptr<Token>>> tokenList;
  int current;
  int lenOfTokenList;
  bool hasError;

public:
  Parser(std::shared_ptr<std::vector<std::unique_ptr<Token>>> const tokenList);
  bool getHasError() const;
  /**
   * Parses the list of Tokens and returns an expression.
   */
  std::shared_ptr<Expr> parse();
  /**
   * Prints out the error to the console and returns a ParseError
   */
  ParseError error(Token const token, std::string const message);

  /*
   * Returns the std::unique_ptr<Expr> object that represents the expression.
   */
  std::shared_ptr<Expr> expression();
  std::shared_ptr<Expr> equality();
  std::shared_ptr<Expr> comparison();
  std::shared_ptr<Expr> term();
  std::shared_ptr<Expr> factor();
  std::shared_ptr<Expr> unary();
  std::shared_ptr<Expr> primary();

  // Helper Functions
  /*
   * Returns whether the current Token's type matches the given TokenType.
   */
  bool check(TokenType const tokenType);
  /*
   * Returns whether the current Token matches one of the given TokenTypes.
   * If it does match, then increment the current index.
   */
  bool match(std::shared_ptr<std::vector<TokenType>> givenTokenTypes);
  /*
  Returns the Token at current index.
  Returns std::nullopt if the current index is out of range.
  */
  std::optional<Token> peek();
  /*
   * Returns the Token at the current index.
   * Increments the current index.
   */
  std::optional<Token> advance();
  /*
   * Matches the given token type else throws an error
   */
  Token consume(TokenType const tokenType, std::string const errorMessage);
  /*  Returns the Token at the previous index.
    Returns std::nullopt if the current index is out of range.
    */
  std::optional<Token> previous();
  /*
   * Returns whether the current inex is in range of the tokenList.
   */
  bool isAtEnd() const;
  /*
   * Returns whether the index is in range of the tokenList.
   */
  bool isInRange(int const index) const;
};
#endif
