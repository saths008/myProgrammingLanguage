#include <iostream>
#include <vector>
#include <any>
#include <memory>
#include "tokentype.hpp"
#include "token.hpp"
#include "scanner.hpp"
using std::cout, std::endl, std::string, std::vector;

Scanner::Scanner(string sourceFile)
{
    this->sourceFile = sourceFile;
    this->hadError = false;
    this->tokenList = std::make_unique<vector<Token *>>();
    this->start, this->current = 0, 0;
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