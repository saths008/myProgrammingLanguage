#ifndef SCANNER_HPP
#define SCANNER_HPP

#include <iostream>
#include <vector>
#include <any>
#include <memory>
#include "tokentype.hpp"
#include "token.hpp"

class Scanner
{
private:
    std::string sourceFile;
    bool hadError;
    std::unique_ptr<std::vector<Token *>> tokenList;
    int start;
    int current;
    int lenOfFile;

public:
    Scanner(std::string sourceFile);
    std::string getSourceFile() const;
    bool getHadError() const;
    int getStart() const;
    int getCurrent() const;
    int getLenOfFile() const;
};

std::ostream &operator<<(std::ostream &s, Scanner const &scanner);

#endif // End of include guard