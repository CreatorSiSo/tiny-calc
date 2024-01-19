#include "parse.hpp"

#include <ranges>

Box<Expr> parse(StringView src, std::span<Token> tokens) {
    auto token = tokens[0];

    if (token.kind == TokenKind::Error) {
        // TODO Emit Report insted
        std::cerr << "Error: Unexpected Token '" << token.src(src) << "'\n";
        return nullptr;
    }

    if (token.kind == TokenKind::Number) {
        return Number::alloc(0.0);
    }

    if (token.kind == TokenKind::Plus) {
        // if (stack.size() < 2) {
        //     std::cerr << "Error: Expected two arguments, but got "
        //               << stack.size() << "\n";
        //     return nullptr;
        // }

        // for (auto& expr : stack) std::cout << *expr << ",\n";

        // Box<Expr> lhs = pop();
        // Box<Expr> rhs = pop();

        // // BinaryExpr::alloc(BinaryOp::Add, std::move(lhs),
        //     std::move(rhs));
    }

    return nullptr;
}

BinaryExpr parse_binary(StringView src, std::span<Token> tokens) {}
