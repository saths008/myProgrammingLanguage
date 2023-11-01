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
    this->currentLine = 1;
    this->lenOfFile = this->sourceFile.length();
    initialiseKeywordMap();
}
std::string Scanner::getSourceFile() const
{
    return sourceFile;
}
void Scanner::scanFile()
{
    this->scanTokens();
    this->printOutTokens();
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

char Scanner::peek()
{
    return isInRange() ? this->sourceFile[this->current] : '\0';
}

char Scanner::advance()
{

    char currentChar = isInRange() ? (this->sourceFile[this->current]) : '\0';
    this->current++;
    return currentChar;
}

void Scanner::scanTokens()
{
    int counter = 0;
    while (isInRange())
    {
        cout << "Counter: " << counter++ << endl;
        cout << "Current: " << this->current << endl;
        this->start = this->current;
        this->scanToken();
    }
}

void Scanner::scanToken()
{
    cout << "In scanToken()" << endl;
    char currentChar = this->advance();
    if (currentChar == '\0')
    {
        return;
    }
    // Match unary operators.
    else if (currentChar == '(')
    {
        this->addToken(TokenType::LEFT_PAREN, this->currentLine, nullptr);
    }
    else if (currentChar == ')')
    {
        this->addToken(TokenType::RIGHT_PAREN, this->currentLine, nullptr);
    }
    else if (currentChar == '{')
    {
        this->addToken(TokenType::LEFT_BRACE, this->currentLine, nullptr);
    }
    else if (currentChar == '}')
    {

        this->addToken(TokenType::RIGHT_BRACE, this->currentLine, nullptr);
    }
    else if (currentChar == ',')
    {
        this->addToken(TokenType::COMMA, this->currentLine, nullptr);
    }
    else if (currentChar == '.')
    {
        this->addToken(TokenType::DOT, this->currentLine, nullptr);
    }
    else if (currentChar == '-')
    {
        this->addToken(TokenType::MINUS, this->currentLine, nullptr);
    }
    else if (currentChar == '+')
    {
        this->addToken(TokenType::PLUS, this->currentLine, nullptr);
    }
    else if (currentChar == '*')
    {
        this->addToken(TokenType::STAR, this->currentLine, nullptr);
    }
    else if (currentChar == ';')
    {
        this->addToken(TokenType::SEMICOLON, this->currentLine, nullptr);
    }
    // Operators with 2 chars
    else if (currentChar == '!')
    {
        if (this->match('='))
        {
            this->addToken(TokenType::BANG_EQUAL, this->currentLine, nullptr);
        }
        else
        {
            this->addToken(TokenType::BANG, this->currentLine, nullptr);
        }
    }
    else if (currentChar == '=')
    {
        if (this->match('='))
        {
            this->addToken(TokenType::EQUAL_EQUAL, this->currentLine, nullptr);
        }
        else
        {
            this->addToken(TokenType::EQUAL, this->currentLine, nullptr);
        }
    }
    else if (currentChar == '<')
    {
        if (this->match('='))
        {
            this->addToken(TokenType::LESS_EQUAL, this->currentLine, nullptr);
        }
        else
        {
            this->addToken(TokenType::LESS, this->currentLine, nullptr);
        }
    }
    else if (currentChar == '>')
    {
        if (this->match('='))
        {
            this->addToken(TokenType::GREATER_EQUAL, this->currentLine, nullptr);
        }
        else
        {
            this->addToken(TokenType::GREATER, this->currentLine, nullptr);
        }
    }
    else if (currentChar == '"')
    {
        bool foundStringEnd = false;
        while ((this->peek() != '\0') && (this->peek() != '"'))
        {
            this->advance();
        }
        if (this->peek() != '\0' && this->peek() == '"')
        {
            foundStringEnd = true;
        }
        if (foundStringEnd)
        {
            this->advance(); // Advance past the closing quote.
            string currentSubstr = this->getCurrentSubstr();
            this->addToken(TokenType::STRING, this->currentLine, currentSubstr);
        }
        else
        {
            string errMssgDesc = "Unterminated String Literal";
            string errorMssg = this->generateError(errMssgDesc);
            this->addError(errorMssg);
        }
    }
    else if (currentChar == '\n')
    {

        this->currentLine++;
    }
    else if (currentChar == ' ')
    {
    }
    else
    {
        if (isdigit(currentChar))
        {
            this->addNumber();
        }
        else if (isalpha(currentChar))
        {
            this->addString();
        }
        else
        {
            cout << "Token: " << currentChar << " not recognised.";
        }
    }
}
void Scanner::addToken(TokenType type, int line, std::any data)
{
    Token *tokenPtr = new Token(type, this->currentLine, data);
    this->tokenList->push_back(tokenPtr);
}
void Scanner::addError(string errorMessage)
{
    string *errorMessagePtr = new string(errorMessage);
    this->errorList->push_back(errorMessagePtr);
}
string Scanner::generateError(string messageDesc) const
{
    string errorMessage = "Error: line " + std::to_string(this->currentLine) + " -" + messageDesc;
    return errorMessage;
}

string Scanner::getCurrentSubstr() const
{
    return this->sourceFile.substr(this->start, this->current - this->start + 1);
}
void Scanner::addNumber()
{
    char currentChar;
    bool isDouble = false;
    while (isInRange() && isdigit(this->peek()))
    {
        currentChar = this->advance();
    }
    if (currentChar == '.')
    {
        isDouble = true;
        this->advance();
    }

    while (isInRange() && isdigit(this->peek()))
    {
        this->advance();
    }
    string numberSubstr = this->getCurrentSubstr();
    if (isDouble)
    {
        double parsedDouble = std::stod(numberSubstr);
        this->addToken(TokenType::NUMBER, this->currentLine, parsedDouble);
    }
    else
    {
        int parsedInt = std::stoi(numberSubstr);
        this->addToken(TokenType::NUMBER, this->currentLine, parsedInt);
    }
}
void Scanner::addString()
{
    string tokenWord = "";
    while (isInRange() && isalpha(this->peek()))
    {
        tokenWord += this->advance();
    }
    TokenType tokenTypeToInsert = this->matchKeyword(tokenWord);
    string dataToInsert = "";
    if (tokenTypeToInsert == TokenType::IDENTIFIER)
    {
        dataToInsert = this->getCurrentSubstr();
    }
    this->addToken(tokenTypeToInsert, this->currentLine, (dataToInsert == "") ? NULL : dataToInsert);
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

void Scanner::printOutTokens() const
{
    for (Token *token : *this->tokenList)
    {
        cout << *token << endl;
    }
}

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