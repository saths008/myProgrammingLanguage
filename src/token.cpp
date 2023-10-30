#include "token.hpp"
#include "tokentype.hpp"
#include <iostream>
#include <any>
Token::Token(TokenType type, int line, std::any data)
{
    this->data = data;
    this->line = line;
    this->type = type;
}

TokenType Token::getType()
{
    return this->type;
}
int Token::getLine()
{
    return this->line;
}

template <typename T>
T Token::getData()
{
    try
    {
        return std::any_cast<T>(data);
    }
    catch (const std::bad_any_cast &e)
    {
        std::cout << e.what() << '\n';
        return NULL;
    }
}