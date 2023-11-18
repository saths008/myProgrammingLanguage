#include "parser.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include <memory>
#include <optional>

using std::vector, std::shared_ptr, std::unique_ptr, std::optional;
Parser::Parser(shared_ptr<vector<unique_ptr<Token>>> const paramTokenList)
    : tokenList(paramTokenList), current(0),
      lenOfTokenList(paramTokenList->size()) {}
bool Parser::check(TokenType const tokenType) {
  optional<Token> optToken = this->peek();
  return optToken ? optToken->getType() == tokenType : false;
}
bool Parser::match(std::shared_ptr<std::vector<TokenType>> givenTokenType) {
  for (TokenType &tokenType : *givenTokenType) {
    if (this->check(tokenType)) {
      this->advance();
      return true;
    }
  }
  return false;
}
optional<Token> Parser::peek() {
  if (isAtEnd()) {
    return std::nullopt;
  }
  return optional<Token>(*((*this->tokenList)[this->current]));
}
optional<Token> Parser::previous() {
  if (this->isInRange(this->current - 1)) {
    return optional<Token>(*((*this->tokenList)[this->current - 1]));
  } else {
    return std::nullopt;
  }
}
optional<Token> Parser::advance() {
  optional<Token> currentToken = this->peek();
  this->current++;
  return currentToken;
}
bool Parser::isAtEnd() const { return this->current >= this->lenOfTokenList; }
bool Parser::isInRange(int const index) const {
  return (index >= 0 && index < this->lenOfTokenList);
}
