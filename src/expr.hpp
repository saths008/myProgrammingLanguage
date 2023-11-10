#ifndef EXPR_HPP
#define EXPR_HPP
#include "token.hpp"
#include "tokentype.hpp"
#include <algorithm>
#include <any>
#include <memory>
#include <string>
#include <variant>
// Define the Expr, Binary, Grouping, Unary, Literal classes
class ExprVisitor;
class PrintVisitor;
class Expr {
public:
  virtual ~Expr() {}
  virtual void accept(std::unique_ptr<ExprVisitor> visitor) = 0;
};

class Binary : public Expr {
private:
  const std::shared_ptr<const Expr> left;
  const std::shared_ptr<const Expr> right;
  const std::shared_ptr<const Token> op;

public:
  Binary(std::shared_ptr<const Expr> left, std::shared_ptr<const Expr> right,
         std::shared_ptr<const Token> op);
  void accept(std::unique_ptr<ExprVisitor> visitor) override;
};
class Grouping : public Expr {
private:
  const std::shared_ptr<const Expr> expr;

public:
  Grouping(std::shared_ptr<const Expr> expr);
  void accept(std::unique_ptr<ExprVisitor> visitor) override;
};
class Unary : public Expr {
private:
  const std::shared_ptr<const Expr> expr;
  const std::shared_ptr<const Token> token;

public:
  Unary(std::shared_ptr<const Expr> expr, std::shared_ptr<const Token> token);
  void accept(std::unique_ptr<ExprVisitor> visitor) override;
};
class Literal : public Expr {
private:
  const std::variant<double, int, std::string> value;

public:
  Literal(std::variant<double, int, std::string> value);
  void accept(std::unique_ptr<ExprVisitor> visitor) override;
};

class ExprVisitor {
public:
  virtual void visit(Grouping &grouping) = 0;
  virtual void visit(Unary &unary) = 0;
  virtual void visit(Binary &binary) = 0;
  virtual void visit(Literal &literal) = 0;
  virtual ~ExprVisitor() = default;
};
class PrintVisitor : public ExprVisitor {

public:
  void visit(Grouping &grouping) override;
  void visit(Unary &unary) override;
  void visit(Binary &binary) override;
  void visit(Literal &literal) override;
  virtual ~PrintVisitor() = default;
};

#endif
