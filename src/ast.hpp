#pragma once

#include "common.hpp"

struct Expr {
    virtual ~Expr();
    virtual string display() const = 0;
};

template <>
struct std::formatter<Expr> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }

    auto format(const Expr& obj, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", obj.display());
    }
};

struct Number : Expr {
    Number(double value);
    static unique_ptr<Expr> alloc(double value);

    string display() const override;

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
    BinaryExpr(BinaryOp op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs);
    static unique_ptr<Expr> alloc(BinaryOp op, unique_ptr<Expr> lhs,
                                  unique_ptr<Expr> rhs);

    string display() const override;

   private:
    BinaryOp m_op;
    unique_ptr<Expr> m_lhs;
    unique_ptr<Expr> m_rhs;
};
