#pragma once

#include "common.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val) : start(start_val), len(len_val) {}

    string debug() const;
    string_view source(string_view str) const;
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

    string_view name();
    string_view src(string_view src);
    string debug(string_view src);
};

std::vector<Token> tokenize(string_view str);
