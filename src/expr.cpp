#include "expr.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::cout, std::endl;
// Binary class
Binary::Binary(std::shared_ptr<const Expr> leftParam,
               std::shared_ptr<const Expr> rightParam,
               std::shared_ptr<const Token> opParam)
    : left(leftParam), right(rightParam), op(opParam) {}
std::shared_ptr<const Token> Binary::getOp() const { return this->op; };
std::shared_ptr<const Expr> Binary::getLeft() const { return this->left; }
std::shared_ptr<const Expr> Binary::getRight() const { return this->right; }
void Binary::accept(const std::shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// Grouping class
Grouping::Grouping(std::shared_ptr<const Expr> exprParam) : expr(exprParam) {}
std::shared_ptr<const Expr> Grouping::getExpr() const { return this->expr; }
void Grouping::accept(const std::shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// Unary class
Unary::Unary(std::shared_ptr<const Expr> exprParam,
             std::shared_ptr<const Token> tokenParam)
    : expr(exprParam), token(tokenParam){};
std::shared_ptr<const Expr> Unary::getExpr() const { return this->expr; }
void Unary::accept(const std::shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
std::shared_ptr<const Token> Unary::getToken() const { return this->token; };
// Literal class
Literal::Literal(std::string valueParam) : value(valueParam) {}
std::string Literal::getValue() const { return this->value; }
void Literal::accept(const std::shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// accept methods
// PrintVisitor methods
void PrintVisitor::visit(const Grouping &grouping) const {
  std::vector<std::shared_ptr<const Expr>> exprs;
  std::shared_ptr<const Expr> expr = grouping.getExpr();
  exprs.push_back(expr);
  paranthesize("Grouping", exprs);
};
void PrintVisitor::visit(const Unary &unary) const {
  std::vector<std::shared_ptr<const Expr>> exprs;
  std::shared_ptr<const Expr> expr = unary.getExpr();
  std::string tokenType = unary.getToken()->stringifyTokenType();
  exprs.push_back(expr);
  paranthesize(tokenType, exprs);

  // cout << "Unary" << endl;
};
void PrintVisitor::visit(const Binary &binary) const {

  std::vector<std::shared_ptr<const Expr>> exprs;
  std::shared_ptr<const Expr> leftExprPtr = binary.getLeft();
  std::shared_ptr<const Expr> rightExprPtr = binary.getRight();
  std::shared_ptr<const Token> opTokenPtr = binary.getOp();
  std::string tokenType = opTokenPtr->stringifyTokenType();
  exprs.push_back(leftExprPtr);
  exprs.push_back(rightExprPtr);
  paranthesize(tokenType, exprs);

  // cout << "Binary" << endl;
};
void PrintVisitor::visit(const Literal &literal) const {
  std::vector<std::shared_ptr<const Expr>> exprs;
  paranthesize(literal.getValue(), exprs);
  // cout << "Literal" << endl;
};
// paranthesize
void paranthesize(const std::string &name,
                  std::vector<std::shared_ptr<const Expr>> &exprs) {
  cout << "(" << name;
  std::shared_ptr<ExprVisitor> exprVisitorPtr =
      std::make_shared<PrintVisitor>();
  for (const std::shared_ptr<const Expr> &expr : exprs) {

    cout << " ";
    expr->accept(exprVisitorPtr);
  }
  cout << ")";
}
