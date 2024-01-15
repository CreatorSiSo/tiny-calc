#pragma once

#include <memory>
#include <ostream>
#include <variant>

template <typename T>
using Box = std::unique_ptr<T>;
using Ostream = std::ostream;

struct Expr {
    virtual Ostream& write(Ostream& stream) const = 0;
    virtual ~Expr() {}
};

Ostream& operator<<(Ostream& out, const Expr& expr);

struct Number : Expr {
    Number(double value);
    static Box<Expr> alloc(double value);

    Ostream& write(Ostream& stream) const;

   private:
    double value;
};

enum class BinaryOp {
    Add,
    Sub,
    Mul,
    Div,
    Cos,
};

struct BinaryExpr : Expr {
    BinaryExpr(BinaryOp op, Box<Expr> lhs, Box<Expr> rhs);
    static Box<Expr> alloc(BinaryOp op, Box<Expr> lhs, Box<Expr> rhs);

    Ostream& write(Ostream& stream) const;

   private:
    BinaryOp op;
    Box<Expr> lhs;
    Box<Expr> rhs;
};
