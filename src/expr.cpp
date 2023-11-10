#include "expr.hpp"
#include <iostream>
#include <memory>
using std::cout, std::endl;
// Constructors for expression
Binary::Binary(std::shared_ptr<const Expr> leftParam,
               std::shared_ptr<const Expr> rightParam,
               std::shared_ptr<const Token> opParam)
    : left(leftParam), right(rightParam), op(opParam) {}

Grouping::Grouping(std::shared_ptr<const Expr> exprParam) : expr(exprParam) {}

Unary::Unary(std::shared_ptr<const Expr> exprParam,
             std::shared_ptr<const Token> tokenParam)
    : expr(exprParam), token(tokenParam){};

Literal::Literal(std::variant<double, int, std::string> valueParam)
    : value(valueParam) {}
// accept methods
void Binary::accept(std::unique_ptr<ExprVisitor> visitor) {
  visitor->visit(*this);
};
void Grouping::accept(std::unique_ptr<ExprVisitor> visitor) {
  visitor->visit(*this);
};
void Unary::accept(std::unique_ptr<ExprVisitor> visitor) {
  visitor->visit(*this);
};
void Literal::accept(std::unique_ptr<ExprVisitor> visitor) {
  visitor->visit(*this);
};
// PrintVisitor methods
void PrintVisitor::visit(Grouping &grouping) { cout << "Grouping" << endl; };
void PrintVisitor::visit(Unary &unary) { cout << "Unary" << endl; };
void PrintVisitor::visit(Binary &binary) { cout << "Binary" << endl; };
void PrintVisitor::visit(Literal &literal) { cout << "Literal" << endl; };
