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

// TODO
struct Token {
    TokenKind kind;
    Span span;

    Token(TokenKind kind_val, Span span_val) : kind(kind_val), span(span_val) {}

    /**
     * @brief Name to be used when displaying tokens.
     * @return The name of its `TokenKind`.
     */
    auto name() const -> std::string_view;

    /**
     * @brief Substring that the `Token` was parsed from.
     * @param source String used to generate the `Token`.
     * @pre @see `Span::source`
     * @return Substring of `string`.
     */
    auto source(std::string_view source) const -> std::string_view;
};

/**
 * @brief Split the source string into tokens.
 * @param source Input source string.
 * @return All valid tokens and errors.
 */
auto tokenize(std::string_view source) -> std::vector<Token>;
