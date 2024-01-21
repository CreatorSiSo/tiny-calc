#pragma once

#include "common.hpp"

struct Number;
struct BinaryExpr;

struct Expr {
    virtual ~Expr();
    virtual auto display() const -> string = 0;
    virtual auto downcast() -> std::variant<BinaryExpr*, Number*>;
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
    static auto alloc(double value) -> unique_ptr<Expr>;

    auto display() const -> string override;
    auto downcast() -> std::variant<BinaryExpr*, Number*> override;

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
    static auto alloc(BinaryOp op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
        -> unique_ptr<Expr>;

    auto display() const -> string override;
    auto downcast() -> std::variant<BinaryExpr*, Number*> override;

   private:
    BinaryOp m_op;
    unique_ptr<Expr> m_lhs;
    unique_ptr<Expr> m_rhs;
};
