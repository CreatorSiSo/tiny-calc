#pragma once

#include "common.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val) : start(start_val), len(len_val) {}

    String debug();
    StringView src(StringView str);
};

enum class TokenKind {
    Plus,
    Minus,
    Star,
    Slash,
    Number,
    Error,
};

struct Token {
    TokenKind kind;
    Span span;

    Token(TokenKind kind_val, Span span_val) : kind(kind_val), span(span_val) {}

    String debug(StringView src);
    StringView src(StringView src);
};

std::vector<Token> tokenize(StringView str);
