#pragma once

#include "common.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val) : start(start_val), len(len_val) {}

    auto debug() const -> string;
    auto source(string_view str) const -> string_view;
};

enum class TokenKind {
    Plus,
    Minus,
    Star,
    Slash,
    Number,
    Error,
    EndOfFile,
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
