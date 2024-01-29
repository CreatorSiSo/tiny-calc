#pragma once

#include "common.hpp"
#include "report.hpp"

enum class TokenKind {
    Ident,
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
    auto src(string_view src) const -> string_view;
    auto debug(string_view src) const -> string;
};

auto tokenize(string_view str) -> vector<Token>;
