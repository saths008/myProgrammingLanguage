#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "tokentype.hpp"
#include <any>

class Token
{
private:
    TokenType type;
    int line;
    std::any data;

public:
    Token(TokenType type, int line, std::any data);

    TokenType getType();
    int getLine();

    template <typename T>
    T getData();
};

#endif
