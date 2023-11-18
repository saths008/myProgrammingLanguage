#include "token.hpp"
#include "tokentype.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <typeinfo>
using std::string;
Token::Token(TokenType type, int line, std::any data) {
  this->data = data;
  this->line = line;
  this->type = type;
}

TokenType Token::getType() const { return this->type; }
int Token::getLine() const { return this->line; }

const std::string Token::stringifyNumberData() const {
  std::unique_ptr<int> dataFromTokenPtr =
      this->getData<int>();        // Attempt to get data as int
  if (dataFromTokenPtr != nullptr) // If data is int
  {
    return std::to_string(*dataFromTokenPtr);
  } else // Attempt to get data as double
  {
    std::unique_ptr<double> dataFromTokenPtr = this->getData<double>();
    return (dataFromTokenPtr == nullptr) ? "Unknown Data Type"
                                         : std::to_string(*dataFromTokenPtr);
  }
}
template <typename T> std::unique_ptr<T> Token::getData() const {
  try {
    return std::make_unique<T>(std::any_cast<T>(data));
  } catch (const std::bad_any_cast &e) {
    std::cout << e.what() << '\n';
    return nullptr;
  }
}
std::string Token::stringifyTokenType() const {
  std::string stringifyTokenType = "";
  // std::string stringifyTokenData = "";
  switch (this->getType()) {
  case LEFT_PAREN:
    stringifyTokenType = "LEFT_PAREN";
    break;

  case RIGHT_PAREN:
    stringifyTokenType = "RIGHT_PAREN";
    break;

  case LEFT_BRACE:
    stringifyTokenType = "LEFT_BRACE";
    break;

  case RIGHT_BRACE:
    stringifyTokenType = "RIGHT_BRACE";
    break;

  case COMMA:
    stringifyTokenType = "COMMA";
    break;

  case DOT:
    stringifyTokenType = "DOT";
    break;

  case SEMICOLON:
    stringifyTokenType = "SEMICOLON";
    break;

  case MINUS:
    stringifyTokenType = "MINUS";
    break;

  case PLUS:
    stringifyTokenType = "PLUS";
    break;

  case SLASH:
    stringifyTokenType = "SLASH";
    break;

  case STAR:
    stringifyTokenType = "STAR";
    break;
  case BANG:
    stringifyTokenType = "BANG";
    break;
  case BANG_EQUAL:
    stringifyTokenType = "BANG_EQUAL";
    break;
  case EQUAL:
    stringifyTokenType = "EQUAL";
    break;
  case EQUAL_EQUAL:
    stringifyTokenType = "EQUAL_EQUAL";
    break;
  case GREATER:
    stringifyTokenType = "GREATER";
    break;
  case GREATER_EQUAL:
    stringifyTokenType = "GREATER_EQUAL";
    break;
  case LESS:
    stringifyTokenType = "LESS";
    break;
  case LESS_EQUAL:
    stringifyTokenType = "LESS_EQUAL";
    break;
  case IDENTIFIER: {
    std::unique_ptr<std::string> dataFromTokenPtr =
        this->getData<std::string>();
    stringifyTokenType = "IDENTIFIER";
  } break;
  case STRING: {

    stringifyTokenType = "STRING";
  } break;
  case NUMBER: {
    // stringifyTokenData = this->stringifyNumberData();
    stringifyTokenType = "NUMBER";
  } break;
  case AND:
    stringifyTokenType = "AND";
    break;
  case CLASS:
    stringifyTokenType = "CLASS";
    break;
  case ELSE:
    stringifyTokenType = "ELSE";
    break;
  case FALSE:
    stringifyTokenType = "FALSE";
    break;
  case FUN:
    stringifyTokenType = "FUN";
    break;
  case FOR:
    stringifyTokenType = "FOR";
    break;
  case IF:
    stringifyTokenType = "IF";
    break;
  case NIL:
    stringifyTokenType = "NIL";
    break;
  case OR:
    stringifyTokenType = "OR";
    break;
  case PRINT:
    stringifyTokenType = "PRINT";
    break;
  case RETURN:
    stringifyTokenType = "RETURN";
    break;
  case SUPER:
    stringifyTokenType = "SUPER";
    break;
  case THIS:
    stringifyTokenType = "THIS";
    break;
  case TRUE:
    stringifyTokenType = "TRUE";
    break;
  case VAR:
    stringifyTokenType = "VAR";
    break;
  case WHILE:
    stringifyTokenType = "WHILE";
    break;
  case ENDOFFILE:
    stringifyTokenType = "ENDOFFILE";
    break;
  }
  return stringifyTokenType;
}
std::string Token::stringifyTokenData() const {
  std::string stringifyTokenData = "";
  switch (this->getType()) {
  case IDENTIFIER: {
    std::unique_ptr<std::string> dataFromTokenPtr =
        this->getData<std::string>();
    stringifyTokenData.append(
        (dataFromTokenPtr == nullptr) ? "No Data" : *dataFromTokenPtr);
  } break;
  case STRING: {

    std::unique_ptr<std::string> dataFromTokenPtr =
        this->getData<std::string>();
    stringifyTokenData.append(
        (dataFromTokenPtr == nullptr) ? "No Data" : *dataFromTokenPtr);
  } break;
  case NUMBER: {
    stringifyTokenData.append(this->stringifyNumberData());
  } break;
  default:
    break;
  }
  return stringifyTokenData;
}
std::string Token::to_string() const {
  std::string stringifyTokenType = this->stringifyTokenType();
  std::string stringifyTokenData = this->stringifyTokenData();
  return "\n{\ntype: TokenType::" + stringifyTokenType + "\n" +
         "tokenData: " + stringifyTokenData + "\n" +
         "line: " + std::to_string(this->getLine()) + "\n}";
}
std::ostream &operator<<(std::ostream &os, const Token &token) {
  os << token.to_string();
  return os;
}
