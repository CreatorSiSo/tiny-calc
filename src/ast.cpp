#include "ast.hpp"

// Expr
Expr::~Expr() = default;

// Number
Number::Number(double value) : m_value(value) {}

unique_ptr<Expr> Number::alloc(double value) {
    return unique_ptr<Expr>(new Number(value));
}

string Number::display() const { return std::format("{}", m_value); }

// BinaryOp
static string_view op_to_str(BinaryOp op) {
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
    // Should never be reached
    std::abort();
}

// BinaryExpr
BinaryExpr::BinaryExpr(BinaryOp op, unique_ptr<Expr> lhs, unique_ptr<Expr> rhs)
    : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

unique_ptr<Expr> BinaryExpr::alloc(BinaryOp op, unique_ptr<Expr> lhs,
                                   unique_ptr<Expr> rhs) {
    return unique_ptr<Expr>(new BinaryExpr(op, std::move(lhs), std::move(rhs)));
}

string BinaryExpr::display() const {
    return std::format("{}({},{})", op_to_str(m_op), *m_lhs, *m_rhs);
}
