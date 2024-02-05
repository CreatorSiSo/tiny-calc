#pragma once

#include "report.hpp"

enum class TokenKind {
    Identifier,
    Plus,
    Minus,
    Star,
    Slash,
    Number,
    Error,
    EndOfInput,
};

struct Token {
    TokenKind kind;
    Span span;

    Token(TokenKind kind_val, Span span_val) : kind(kind_val), span(span_val) {}
    auto name() const -> std::string_view;
    auto source(std::string_view source) const -> std::string_view;
};

auto tokenize(std::string_view source) -> std::vector<Token>;
