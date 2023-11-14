#include "expr.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
using std::cout, std::shared_ptr, std::string, std::vector;
// Binary class
Binary::Binary(shared_ptr<const Expr> leftParam,
               shared_ptr<const Expr> rightParam,
               shared_ptr<const Token> opParam)
    : left(leftParam), right(rightParam), op(opParam) {}
shared_ptr<const Token> Binary::getOp() const { return this->op; };
shared_ptr<const Expr> Binary::getLeft() const { return this->left; }
shared_ptr<const Expr> Binary::getRight() const { return this->right; }
void Binary::accept(const shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// Grouping class
Grouping::Grouping(shared_ptr<const Expr> exprParam) : expr(exprParam) {}
shared_ptr<const Expr> Grouping::getExpr() const { return this->expr; }
void Grouping::accept(const shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// Unary class
Unary::Unary(shared_ptr<const Expr> exprParam,
             shared_ptr<const Token> tokenParam)
    : expr(exprParam), token(tokenParam){};
shared_ptr<const Expr> Unary::getExpr() const { return this->expr; }
void Unary::accept(const shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
shared_ptr<const Token> Unary::getToken() const { return this->token; };
// Literal class
Literal::Literal(string valueParam) : value(valueParam) {}
string Literal::getValue() const { return this->value; }
void Literal::accept(const shared_ptr<ExprVisitor> visitor) const {
  visitor->visit(*this);
};
// accept methods
// PrintVisitor methods
void PrintVisitor::visit(const Grouping &grouping) const {
  vector<shared_ptr<const Expr>> exprs;
  shared_ptr<const Expr> expr = grouping.getExpr();
  exprs.push_back(expr);
  paranthesize("Grouping", exprs);
};
void PrintVisitor::visit(const Unary &unary) const {
  vector<shared_ptr<const Expr>> exprs;
  shared_ptr<const Expr> expr = unary.getExpr();
  string tokenType = unary.getToken()->stringifyTokenType();
  exprs.push_back(expr);
  paranthesize(tokenType, exprs);

  // cout << "Unary" << endl;
};
void PrintVisitor::visit(const Binary &binary) const {

  vector<shared_ptr<const Expr>> exprs;
  shared_ptr<const Expr> leftExprPtr = binary.getLeft();
  shared_ptr<const Expr> rightExprPtr = binary.getRight();
  shared_ptr<const Token> opTokenPtr = binary.getOp();
  string tokenType = opTokenPtr->stringifyTokenType();
  exprs.push_back(leftExprPtr);
  exprs.push_back(rightExprPtr);
  paranthesize(tokenType, exprs);

  // cout << "Binary" << endl;
};
void PrintVisitor::visit(const Literal &literal) const {
  vector<shared_ptr<const Expr>> exprs;
  paranthesize(literal.getValue(), exprs);
  // cout << "Literal" << endl;
};
// paranthesize
void paranthesize(const string &name, vector<shared_ptr<const Expr>> &exprs) {
  cout << "(" << name;
  shared_ptr<ExprVisitor> exprVisitorPtr = std::make_shared<PrintVisitor>();
  for (const shared_ptr<const Expr> &expr : exprs) {

    cout << " ";
    expr->accept(exprVisitorPtr);
  }
  cout << ")";
}
