#pragma once

#include "common.hpp"

struct Expr {
    virtual ~Expr();
    virtual Ostream& write(Ostream& stream) const = 0;
};

Ostream& operator<<(Ostream& out, const Expr& expr);

struct Number : Expr {
    Number(double value);
    static Box<Expr> alloc(double value);

    Ostream& write(Ostream& stream) const override;

   private:
    double m_value;
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

    Ostream& write(Ostream& stream) const override;

   private:
    BinaryOp m_op;
    Box<Expr> m_lhs;
    Box<Expr> m_rhs;
};
