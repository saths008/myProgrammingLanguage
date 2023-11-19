#include "parser.hpp"
#include "expr.hpp"
#include "scanner.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include <iostream>
#include <memory>
#include <optional>
#include <vector>
using std::vector, std::shared_ptr, std::unique_ptr, std::optional, std::string,
    std::cout, std::endl;
Parser::Parser(shared_ptr<vector<unique_ptr<Token>>> const paramTokenList)
    : tokenList(paramTokenList), current(0),
      lenOfTokenList(paramTokenList->size()), hasError(false) {}

shared_ptr<Expr> Parser::parse() {
  try {
    return this->expression();
  } catch (ParseError error) {
    return nullptr;
  }
};
shared_ptr<Expr> Parser::expression() { return this->equality(); };

shared_ptr<Expr> Parser::equality() {
  shared_ptr<Expr> expr = this->comparison();
  vector<TokenType> tokenTypes = {TokenType::BANG_EQUAL,
                                  TokenType::EQUAL_EQUAL};
  while (this->match(std::make_shared<vector<TokenType>>(tokenTypes))) {
    optional<Token> optToken = this->previous();
    shared_ptr<Token> operatorTokenPtr =
        optToken ? std::make_shared<Token>(optToken.value()) : nullptr;
    shared_ptr<Expr> right = this->comparison();
    expr = std::make_shared<Binary>(expr, right, operatorTokenPtr);
  };
  return expr;
};
shared_ptr<Expr> Parser::comparison() {
  shared_ptr<Expr> expr = this->term();
  vector<TokenType> tokenTypes = {TokenType::GREATER, TokenType::GREATER_EQUAL,
                                  TokenType::LESS, TokenType::LESS_EQUAL};
  while (this->match(std::make_shared<vector<TokenType>>(tokenTypes))) {
    optional<Token> optToken = this->previous();
    shared_ptr<Token> operatorTokenPtr =
        optToken ? std::make_shared<Token>(optToken.value()) : nullptr;
    shared_ptr<Expr> right = this->term();
    expr = std::make_shared<Binary>(expr, right, operatorTokenPtr);
  };
  return expr;
};
shared_ptr<Expr> Parser::term() {
  shared_ptr<Expr> expr = this->factor();
  vector<TokenType> tokenTypes = {TokenType::MINUS, TokenType::PLUS};
  while (this->match(std::make_shared<vector<TokenType>>(tokenTypes))) {
    optional<Token> optToken = this->previous();
    shared_ptr<Token> operatorTokenPtr =
        optToken ? std::make_shared<Token>(optToken.value()) : nullptr;
    shared_ptr<Expr> right = this->factor();
    expr = std::make_shared<Binary>(expr, right, operatorTokenPtr);
  };
  return expr;
};
shared_ptr<Expr> Parser::factor() {
  shared_ptr<Expr> expr = this->unary();
  vector<TokenType> tokenTypes = {TokenType::SLASH, TokenType::STAR};
  while (this->match(std::make_shared<vector<TokenType>>(tokenTypes))) {
    optional<Token> optToken = this->previous();
    shared_ptr<Token> operatorTokenPtr =
        optToken ? std::make_shared<Token>(optToken.value()) : nullptr;
    shared_ptr<Expr> right = this->unary();
    expr = std::make_shared<Binary>(expr, right, operatorTokenPtr);
  };
  return expr;
};
shared_ptr<Expr> Parser::unary() {
  vector<TokenType> tokenTypes = {TokenType::BANG, TokenType::MINUS};
  if (this->match(std::make_shared<vector<TokenType>>(tokenTypes))) {
    optional<Token> optToken = this->previous();
    shared_ptr<Token> operatorTokenPtr =
        optToken ? std::make_shared<Token>(optToken.value()) : nullptr;
    shared_ptr<Expr> right = this->unary();
    return std::make_shared<Unary>(right, operatorTokenPtr);
  };
  return this->primary();
};
shared_ptr<Expr> Parser::primary() {

  vector<TokenType> givenTokenTypes = {TokenType::FALSE};
  shared_ptr<vector<TokenType>> givenTokenTypePtr =
      std::make_shared<vector<TokenType>>(givenTokenTypes);
  if (this->match(givenTokenTypePtr)) {
    return std::make_shared<Literal>("false");
  }
  (*givenTokenTypePtr)[0] = TokenType::TRUE;
  if (this->match(givenTokenTypePtr)) {
    return std::make_shared<Literal>("true");
  }
  (*givenTokenTypePtr)[0] = TokenType::NIL;
  if (this->match(givenTokenTypePtr)) {
    return std::make_shared<Literal>(nullptr);
  }

  (*givenTokenTypePtr)[0] = TokenType::NUMBER;
  givenTokenTypePtr->emplace_back(TokenType::STRING);
  if (this->match(givenTokenTypePtr)) {
    optional<Token> optToken = this->previous();
    string numberData = optToken ? optToken->stringifyTokenData() : nullptr;
    return std::make_shared<Literal>(numberData);
  }
  givenTokenTypePtr->pop_back();
  (*givenTokenTypePtr)[0] = TokenType::LEFT_PAREN;
  if (this->match(givenTokenTypePtr)) {
    shared_ptr<Expr> expr = this->expression();
    this->match(std::make_shared<vector<TokenType>>(
        vector<TokenType>{TokenType::RIGHT_PAREN}));
    return std::make_shared<Grouping>(expr);
  }
  throw this->error(this->peek().value(), "Expect expression.");
};
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
ParseError Parser::error(Token const token, std::string const message) {
  this->hasError = true;
  cout << "Error: "
       << "Line " << token.getLine() << message << endl;
  return ParseError(message);
}
Token Parser::consume(TokenType const tokenType,
                      std::string const errorMessage) {
  if (this->check(tokenType)) {
    return this->advance().value();
  }
  throw this->error(this->peek().value(), errorMessage);
}
bool Parser::isAtEnd() const { return this->current >= this->lenOfTokenList; }
bool Parser::isInRange(int const index) const {
  return (index >= 0 && index < this->lenOfTokenList);
}
bool Parser::getHasError() const { return this->hasError; }
