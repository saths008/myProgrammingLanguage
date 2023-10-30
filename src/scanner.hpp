#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <iostream>
#include <vector>
#include <any>
#include <memory>
#include "tokentype.hpp"
#include "token.hpp"
#include <unordered_map>
class Scanner
{
private:
    std::string sourceFile;
    bool hadError;
    std::unique_ptr<std::vector<Token *>> tokenList;
    std::unique_ptr<std::unordered_map<std::string, TokenType>> keywordMap;
    int start;
    int current;
    int lenOfFile;
    int currentLine;

public:
    Scanner(std::string sourceFile);
    std::string getSourceFile() const;
    bool getHadError() const;
    int getStart() const;
    int getCurrent() const;
    int getLenOfFile() const;
    int getCurrentLine() const;
    void initialiseKeywordMap();
    TokenType matchKeyword(std::string tokenWord);
    void scanTokens();
    std::unique_ptr<string> getCurrentSubstr() const;
    void scanToken();
    void printOutTokens() const;
    /*
    Checks if the current index is in
    range of the source file
    */
    bool isInRange() const;
    /*
    Checks if the param is in
    range of the source file
    */
    bool isInRange(int input) const;
    /*
    Returns the char at the next index.
    Returns NULL if the next char is
    out of bounds.
    */
    char *peek();
    /*
    Returns the char if current is now
    in range.
    Returns NULL if current is now out
    of bounds.

    Also increments the current pointer.
    */
    char *advance();
    /*
    Check if the char at the current
    pointer is equal to expected.

    If it is true, then also increment current.
    */
    bool match(char const expected);

    void addNumber();
    void addString();
};

std::ostream &operator<<(std::ostream &s, Scanner const &scanner);

#endif // End of include guard