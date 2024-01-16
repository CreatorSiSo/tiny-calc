#include "ast.hpp"

// Expr
Expr::~Expr() = default;
Ostream& operator<<(Ostream& out, const Expr& expr) { return expr.write(out); }

// Number
Number::Number(double value) : m_value(value) {}

Box<Expr> Number::alloc(double value) { return Box<Expr>(new Number(value)); }

Ostream& Number::write(Ostream& stream) const { return stream << m_value; }

// BinaryOp
static StringView op_to_str(BinaryOp op) {
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
BinaryExpr::BinaryExpr(BinaryOp op, Box<Expr> lhs, Box<Expr> rhs)
    : m_op(op), m_lhs(std::move(lhs)), m_rhs(std::move(rhs)) {}

Box<Expr> BinaryExpr::alloc(BinaryOp op, Box<Expr> lhs, Box<Expr> rhs) {
    return Box<Expr>(new BinaryExpr(op, std::move(lhs), std::move(rhs)));
}

Ostream& BinaryExpr::write(Ostream& stream) const {
    return stream << op_to_str(m_op) << "(" << *m_lhs << ", " << *m_rhs << ")";
}
