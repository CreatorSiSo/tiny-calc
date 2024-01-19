#include "parse.hpp"

#include <ranges>

unique_ptr<Expr> parse(string_view src, std::span<Token> tokens) {
    auto token = tokens[0];

    if (token.kind == TokenKind::Error) {
        // TODO Emit Report insted
        writeln(std::cerr, "Error: Unexpected Token '{}'\n", token.src(src));
        return nullptr;
    }

    if (token.kind == TokenKind::Number) {
        return Number::alloc(0.0);
    }

    if (token.kind == TokenKind::Plus) {
        // vector<uint8_t> stack = {};

        // if (stack.size() < 2) {
        //     writeln(std::cerr, "Error: Expected two arguments, but got {}",
        //             stack.size());
        //     return nullptr;
        // }

        // for (auto& expr : stack) println("{},", *expr);

        // unique_ptr<Expr> lhs = pop();
        // unique_ptr<Expr> rhs = pop();

        // // BinaryExpr::alloc(BinaryOp::Add, std::move(lhs),
        //     std::move(rhs));
    }

    return nullptr;
}

// BinaryExpr parse_binary(string_view src, std::span<Token> tokens) {}
