#include "ast.hpp"

// Expr
Expr::~Expr() = default;

// Number
Number::Number(double value) : m_value(value) {}

auto Number::alloc(double value) -> unique_ptr<Expr> {
    return unique_ptr<Expr>(new Number(value));
}

auto Number::display() const -> string { return std::format("{}", m_value); }

auto Number::downcast() -> std::variant<BinaryExpr*, Number*> { return this; }

// BinaryOp
static auto op_to_str(BinaryOp op) -> string_view {
    switch (op) {
        case BinaryOp::Add:
            return "Add";
        case BinaryOp::Sub:
            return "Sub";
        case BinaryOp::Mul:
            return "Mul";
        case BinaryOp::Div:
            return "Div";
        case BinaryOp::Cos:
            return "Cos";
    }

    panic("unreachable");
}

// BinaryExpr
BinaryExpr::BinaryExpr(BinaryOp op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
    : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

auto BinaryExpr::alloc(BinaryOp op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
    -> unique_ptr<Expr> {
    return unique_ptr<Expr>(new BinaryExpr(op, std::move(lhs), std::move(rhs)));
}

auto BinaryExpr::display() const -> string {
    return std::format("{}({},{})", op_to_str(m_op), *m_lhs, *m_rhs);
}

auto BinaryExpr::downcast() -> std::variant<BinaryExpr*, Number*> {
    return this;
}
