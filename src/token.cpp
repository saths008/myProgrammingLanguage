#include "tokentype.hpp"
#include <iostream>
#include <any>
class Token
{
private:
    TokenType type;
    int line;
    std::any data;

public:
    Token(TokenType type, int line, std::any data)
    {
        this->data = data;
        this->line = line;
        this->type = type;
    }

    TokenType getType()
    {
        return this->type;
    }
    int getLine()
    {
        return this->line;
    }

    template <typename T>
    T getData()
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
};