#include "ast.hpp"

// Expr
Ostream& operator<<(Ostream& out, const Expr& expr) { return expr.write(out); }

// Number
Number::Number(double value) : value(value) {}

Box<Expr> Number::alloc(double value) { return Box<Expr>(new Number(value)); }

Ostream& Number::write(Ostream& stream) const { return stream << value; }

// BinaryOp
std::string_view op_to_str(BinaryOp op) {
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
    : op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}

Box<Expr> BinaryExpr::alloc(BinaryOp op, Box<Expr> lhs, Box<Expr> rhs) {
    return Box<Expr>(new BinaryExpr(op, std::move(lhs), std::move(rhs)));
}

Ostream& BinaryExpr::write(Ostream& stream) const {
    return stream << op_to_str(op) << "(" << *lhs << ", " << *rhs << ")";
}
