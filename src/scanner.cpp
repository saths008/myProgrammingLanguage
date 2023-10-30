#include <iostream>
#include <vector>
#include <any>
#include <memory>
#include "tokentype.hpp"
#include "token.hpp"
#include "scanner.hpp"
#include <string.h>
#include <ctype.h>
#include <unordered_map>
using std::cout, std::endl, std::string, std::vector;

Scanner::Scanner(string sourceFile)
{
    this->sourceFile = sourceFile;
    this->hadError = false;
    this->tokenList = std::make_unique<vector<Token *>>();
    this->start = 0;
    this->current = 0;
    this->currentLine = 0;
    this->lenOfFile = this->sourceFile.length();
}
std::string Scanner::getSourceFile() const
{
    return sourceFile;
}

bool Scanner::getHadError() const
{
    return hadError;
}

int Scanner::getStart() const
{
    return start;
}

int Scanner::getCurrent() const
{
    return current;
}

int Scanner::getLenOfFile() const
{
    return lenOfFile;
}
int Scanner::getCurrentLine() const
{
    return this->currentLine;
}
/**
 * Checks if the current index is within range of the source
 * file string.
 */
bool Scanner::isInRange() const
{
    return isInRange(this->current);
}
/**
 * Checks if the input is within range of the source
 * file string.
 */
bool Scanner::isInRange(int input) const
{
    return input <= this->lenOfFile - 1;
}

char *Scanner::peek()
{
    return isInRange() ? &(this->sourceFile[this->current]) : nullptr;
}

char *Scanner::advance()
{

    char *currentCharPtr = isInRange() ? &(this->sourceFile[this->current]) : nullptr;
    this->current++;
    return currentCharPtr;
}

void Scanner::scanTokens()
{

    while (isInRange())
    {
        this->start = this->current;
        this->scanToken();
    }
}

void Scanner::scanToken()
{
    char *currentCharPtr = this->advance();
    if (currentCharPtr == nullptr)
    {
        return;
    }
    // Match unary operators.
    else if (*currentCharPtr == '(')
    {
        Token token(TokenType::LEFT_PAREN, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == ')')
    {
        Token token(TokenType::RIGHT_PAREN, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '{')
    {
        Token token(TokenType::LEFT_BRACE, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '}')
    {
        Token token(TokenType::RIGHT_BRACE, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == ',')
    {
        Token token(TokenType::COMMA, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '.')
    {
        Token token(TokenType::DOT, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '-')
    {
        Token token(TokenType::MINUS, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '+')
    {
        Token token(TokenType::PLUS, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == '*')
    {
        Token token(TokenType::STAR, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    else if (*currentCharPtr == ';')
    {
        Token token(TokenType::SEMICOLON, this->currentLine, NULL);
        this->tokenList->push_back(&token);
    }
    // Operators with 2 chars
    else if (*currentCharPtr == '!')
    {
        if (this->match('='))
        {
            Token token(TokenType::BANG_EQUAL, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
        else
        {
            Token token(TokenType::BANG, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
    }
    else if (*currentCharPtr == '=')
    {
        if (this->match('='))
        {
            Token token(TokenType::EQUAL_EQUAL, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
        else
        {
            Token token(TokenType::EQUAL, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
    }
    else if (*currentCharPtr == '<')
    {
        if (this->match('='))
        {
            Token token(TokenType::LESS_EQUAL, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
        else
        {
            Token token(TokenType::LESS, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
    }
    else if (*currentCharPtr == '>')
    {
        if (this->match('='))
        {
            Token token(TokenType::GREATER_EQUAL, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
        else
        {
            Token token(TokenType::GREATER, this->currentLine, NULL);
            this->tokenList->push_back(&token);
        }
    }
    else
    {
        if (isdigit(*currentCharPtr))
        {
            this->addNumber();
        }
        else if (isalpha(*currentCharPtr))
        {
            this->addString();
        }
        else
        {
            cout << "Token: " << *currentCharPtr << " not recognised.";
        }
    }
}

std::unique_ptr<string> Scanner::getCurrentSubstr() const
{
    return std::make_unique<string>(this->sourceFile.substr(this->start, this->current - this->start + 1));
}
void Scanner::addNumber()
{
    char *currentChar;
    bool isDouble = false;
    while (isInRange() && isdigit(*this->peek()))
    {
        currentChar = this->advance();
    }
    if (*currentChar == '.')
    {
        isDouble = true;
        this->advance();
    }

    while (isInRange() && isdigit(*this->peek()))
    {
        this->advance();
    }
    string numberSubstr = *this->getCurrentSubstr();
    if (isDouble)
    {
        double parsedDouble = std::stod(numberSubstr);
        Token token(TokenType::NUMBER, this->currentLine, parsedDouble);
        this->tokenList->push_back(&token);
    }
    else
    {
        int parsedInt = std::stoi(numberSubstr);
        Token token(TokenType::NUMBER, this->currentLine, parsedInt);
        this->tokenList->push_back(&token);
    }
}
void Scanner::addString()
{
    string tokenWord;
    while (isInRange() && isalpha(*this->peek()))
    {
        tokenWord += this->advance();
    }
    TokenType tokenTypeToInsert = this->matchKeyword(tokenWord);
    string dataToInsert = NULL;
    if (tokenTypeToInsert == TokenType::IDENTIFIER)
    {
        dataToInsert = *this->getCurrentSubstr();
    }
    Token token(tokenTypeToInsert, this->currentLine, dataToInsert);
    this->tokenList->push_back(&token);
}
TokenType Scanner::matchKeyword(string tokenWord)
{
    TokenType typeToInsert;
    try
    {
        typeToInsert = this->keywordMap->at(tokenWord);
    }
    catch (const std::out_of_range &e)
    {
        std::cerr << "Key not found: " << e.what() << std::endl;
        typeToInsert = TokenType::IDENTIFIER;
    }
    return typeToInsert;
}
void Scanner::initialiseKeywordMap()
{
    std::unordered_map<std::string, TokenType> localKeywordMap =
        {
            {"and", TokenType::AND},
            {"class", TokenType::CLASS},
            {"else", TokenType::ELSE},
            {"false", TokenType ::FALSE},
            {"for", TokenType::FOR},
            {"fun", TokenType::FUN},
            {"if", TokenType::IF},
            {"null", TokenType::NIL},
            {"or", TokenType::OR},
            {"print", TokenType::PRINT},
            {"return", TokenType::RETURN},
            {"super", TokenType::SUPER},
            {"this", TokenType::THIS},
            {"true", TokenType::TRUE},
            {"var", TokenType::VAR},
            {"while", TokenType::WHILE}};

    this->keywordMap = std::make_unique<std::unordered_map<std::string, TokenType>>(localKeywordMap);
}
bool Scanner::match(char const expected)
{
    if (!this->isInRange())
    {
        return false;
    }
    else if (this->sourceFile[current] != expected)
    {
        return false;
    }
    else
    {
        this->current++;
        return true;
    }
}

// void Scanner::printOutTokens() const
// {
//     for (Token *token : *this->tokenList)
//     {
//         if (token->getType() == TokenType::NUMBER)
//         {
//         }
//         else if (token->getType() == TokenType::IDENTIFIER)
//         {
//         }
//     }
// }
std::ostream &operator<<(std::ostream &os, const Scanner &scanner)
{
    os << "   {"
       << "\n"
       << "             sourceFile: " << scanner.getSourceFile() << "\n"
       << "             start: " << scanner.getStart() << "\n"
       << "             current: " << scanner.getCurrent() << "\n"
       << "             lenOfFile: " << scanner.getLenOfFile() << "\n"
       << "             }";
    return os;
}