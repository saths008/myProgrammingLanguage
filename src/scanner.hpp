#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "token.hpp"
#include "tokentype.hpp"
#include <any>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>
class Scanner {
private:
  std::string sourceFile;
  bool hadError;
  std::shared_ptr<std::vector<std::unique_ptr<Token>>> tokenList;
  std::unique_ptr<std::vector<std::string>> errorList;
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
  // Returns the TokenType of the keyword or IDENTIFIER if it is not a keyword
  TokenType matchKeyword(std::string tokenWord);
  void scanTokens();
  // Returns the substring from start to current
  std::string getCurrentSubstr() const;
  std::string getCurrentSubstr(int start, int end) const;
  void scanToken();
  void printOutTokens() const;
  void printOutErrors() const;
  std::string generateError(std::string message) const;
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
  Returns the char at current index.
  Returns '/O' if the char is
  out of bounds.
  */
  char peek();
  /*
  Returns the char if current is now
  in range.
  Returns '/O' if current is now out
  of bounds.

  Then, increments the current pointer.
  */
  char advance();
  /*
  Check if the char at the current
  pointer is equal to expected.

  If it is true, then also increment current.
  */
  bool match(char const expected);

  // Attempt to add a number (float or double) to the tokenList
  void addNumber();
  // Attempt to add a string to the tokenList
  void addString();
  // Add token to tokenList field
  void addToken(TokenType type, int line, std::any data);
  // Add error message to errorList field
  void addError(std::string errorMessage);
  // Scans all the tokens, then prints them out. Then, prints out the list of
  // errors.
  void scanFile();
};

std::ostream &operator<<(std::ostream &s, Scanner const &scanner);

#endif // End of include guard
