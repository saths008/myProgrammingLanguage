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
  virtual void accept(std::shared_ptr<ExprVisitor> visitor) const = 0;
};

class Binary : public Expr {
private:
  const std::shared_ptr<const Expr> left;
  const std::shared_ptr<const Expr> right;
  const std::shared_ptr<const Token> op;

public:
  Binary(std::shared_ptr<const Expr> left, std::shared_ptr<const Expr> right,
         std::shared_ptr<const Token> op);
  void accept(std::shared_ptr<ExprVisitor> visitor) const override;
  std::shared_ptr<const Expr> getLeft() const;
  std::shared_ptr<const Expr> getRight() const;
  std::shared_ptr<const Token> getOp() const;
};
class Grouping : public Expr {
private:
  const std::shared_ptr<const Expr> expr;

public:
  Grouping(std::shared_ptr<const Expr> expr);
  void accept(std::shared_ptr<ExprVisitor> visitor) const override;
  std::shared_ptr<const Expr> getExpr() const;
};
class Unary : public Expr {
private:
  const std::shared_ptr<const Expr> expr;
  const std::shared_ptr<const Token> token;

public:
  Unary(std::shared_ptr<const Expr> expr, std::shared_ptr<const Token> token);
  void accept(std::shared_ptr<ExprVisitor> visitor) const override;
  std::shared_ptr<const Expr> getExpr() const;
  std::shared_ptr<const Token> getToken() const;
};
class Literal : public Expr {
private:
  const std::string value; // use stringifyNumberData() to get the value

public:
  Literal(std::string value);
  void accept(std::shared_ptr<ExprVisitor> visitor) const override;
  std::string getValue() const;
};

class ExprVisitor {
public:
  virtual void visit(const Grouping &grouping) const = 0;
  virtual void visit(const Unary &unary) const = 0;
  virtual void visit(const Binary &binary) const = 0;
  virtual void visit(const Literal &literal) const = 0;
  virtual ~ExprVisitor() = default;
};
class PrintVisitor : public ExprVisitor {

public:
  void visit(const Grouping &grouping) const override;
  void visit(const Unary &unary) const override;
  void visit(const Binary &binary) const override;
  void visit(const Literal &literal) const override;
  virtual ~PrintVisitor() = default;
};
void paranthesize(const std::string &name,
                  std::vector<std::shared_ptr<const Expr>> &exprs);

#endif
