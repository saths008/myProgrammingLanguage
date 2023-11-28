#include "expr.hpp"
#include "parser.hpp"
#include "scanner.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::cout, std::endl, std::string, std::vector, std::shared_ptr,
    std::unique_ptr;
void run(string program) {
  Scanner scanner(program);
  shared_ptr<vector<unique_ptr<Token>>> listOfTokens = scanner.scanFile();
  cout << "Scanning done" << endl;
  if (scanner.getHadError()) {
    cout << "Scanner had error" << endl;
    return;
  }
  unique_ptr<Parser> parserPtr = std::make_unique<Parser>(listOfTokens);
  shared_ptr<Expr> exprPtr = parserPtr->parse();
  cout << "Parsing done" << endl;
  if (parserPtr->getHasError()) {
    cout << "Parser had error" << endl;
    return;
  }
  std::unique_ptr<PrintVisitor> printVisitor = std::make_unique<PrintVisitor>();
  cout << "Printing starting" << endl;
  exprPtr->accept(std::move(printVisitor));
  cout << "Printing done" << endl;
}
string readFile(string filePath) {
  std::ifstream MyReadFile(filePath);
  std::string wholeFile;
  std::string line;
  while (std::getline(MyReadFile, line)) {
    wholeFile = wholeFile + line + "\n";
  }
  return wholeFile;
}
void debugMain() {
  string fileRead = readFile("hello.saath");
  run(fileRead);
}
int main(int argc, char *argv[]) {
  cout << "Number of Arguments: " << argc << endl;
  for (int i = 0; i < argc; i++) {
    char *ptr = argv[i];
    cout << "Argument " << i << " :" << ptr << endl;
  }
  // debugMain();
  if (argc > 1 && std::filesystem::exists(argv[1])) {
    string fileRead = readFile(argv[1]);
    run(fileRead);
  } else if (argc == 1) {
    std::string line;
    std::cout << "Welcome to the REPL! \n";
    while (true) {
      std::cout << "> ";
      std::getline(std::cin, line);

      if (line.empty()) {
        break;
      }
      std::cout << "You entered: " << line << std::endl;
      run(line);
    }
  }
  cout << "Exiting........" << endl;
  return 0;
}
