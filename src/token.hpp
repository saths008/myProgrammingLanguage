#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "tokentype.hpp"
#include <any>
#include <memory>

class Token {
private:
  TokenType type;
  int line;
  std::any data;

public:
  Token(TokenType type, int line, std::any data);

  TokenType getType() const;
  int getLine() const;

  template <typename T> std::unique_ptr<T> getData() const;

  std::string *stringifyToken() const;
  // Return a string containing the data of the token
  std::string stringifyTokenData() const;
  // Return the type of a token as a string
  std::string stringifyTokenType() const;
  std::string to_string() const;
  const std::string stringifyNumberData() const;
};
std::ostream &operator<<(std::ostream &s, Token const &token);
#endif
