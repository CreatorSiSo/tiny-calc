#pragma once

#include "common.hpp"
#include "report.hpp"
#include "utf8.hpp"

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

    auto name() const -> string_view;
    auto source(string_view src) const -> string_view;
};

auto tokenize(string_view str) -> vector<Token>;
