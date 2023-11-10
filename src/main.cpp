#include "expr.hpp"
#include "scanner.hpp"
#include <any>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <variant>
using std::cout, std::endl, std::string;
void run(string program) {
  Scanner scanner(program);
  scanner.scanFile();
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

int main(int argc, char *argv[]) {
  // cout << "Number of Arguments: " << argc << endl;
  // for (int i = 0; i < argc; i++) {
  //   char *ptr = argv[i];
  //   cout << "Argument " << i << " :" << ptr << endl;
  // }
  // if (argc > 1 && std::filesystem::exists(argv[1])) {
  //   string fileRead = readFile(argv[1]);
  //   run(fileRead);
  // } else if (argc == 1) {
  //   std::string line;
  //   std::cout << "Welcome to the REPL! \n";
  //   while (true) {
  //     std::cout << "> ";
  //     std::getline(std::cin, line);
  //
  //     if (line.empty()) {
  //       break;
  //     }
  //     std::cout << "You entered: " << line << std::endl;
  //     run(line);
  //   }
  // }
  std::any data = 1;
  auto unaryToken = std::make_shared<Token>(TokenType::MINUS, 2, "");
  auto unaryLiteral = std::make_shared<Literal>("123");
  auto starOp = std::make_shared<Token>(TokenType::STAR, 2, "");
  auto groupingBinary =
      std::make_shared<Grouping>(std::make_shared<Literal>("45.67"));
  auto unaryPtr = std::make_shared<Unary>(unaryLiteral, unaryToken);
  std::unique_ptr<Expr> binaryPtr =
      std::make_unique<Binary>(unaryPtr, groupingBinary, starOp);
  std::unique_ptr<PrintVisitor> printVisitor = std::make_unique<PrintVisitor>();
  binaryPtr->accept(std::move(printVisitor));
  return 0;
}
