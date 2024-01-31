#include "tokenize.hpp"

#include <cctype>

#include "utf8.hpp"

auto Token::name() const -> string_view {
    switch (kind) {
        case TokenKind::Identifier:
            return "Identifier";
        case TokenKind::Plus:
            return "Plus";
        case TokenKind::Minus:
            return "Minus";
        case TokenKind::Star:
            return "Star";
        case TokenKind::Slash:
            return "Slash";
        case TokenKind::Number:
            return "Number";
        case TokenKind::Error:
            return "Error";
        case TokenKind::EndOfInput:
            return "EndOfInput";
        default:
            panic("Case <{}> not covered", (uint8_t)kind);
    }
}

auto Token::source(string_view src) const -> string_view {
    return span.source(src);
}

/**
 * @brief Try to consume as many whitespace characters as possible.
 * @param source Slice of the input string that is to be checked.
 * @return Length, in bytes, of the consumed characters.
 */
static auto validate_whitespace(string_view source) -> size_t {
    size_t len = 0;
    for (char chr : source) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

/**
 * @brief Try to consume as many characters, of this syntax:
 * `
 *   digit ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9";
 *   number ::= digit (digit | "_")* ("." (digit | "_")*)?;
 * `
 * @param source Slice of the input string that is to be checked.
 * @return Length, in bytes, of the consumed characters.
 */
static auto validate_number(string_view source) -> size_t {
    if (!std::isdigit(source.at(0))) return 0;

    size_t length = 1;
    auto integers = source.substr(length);
    bool has_decimal_part = false;

    auto valid_char = [](char c) { return std::isdigit(c) || c == '_'; };

    for (char c : integers) {
        if (c == '.') {
            has_decimal_part = true;
            length += 1;
            break;
        }
        if (!valid_char(c)) break;
        length += 1;
    }

    if (has_decimal_part) {
        for (char c : source.substr(length)) {
            if (!valid_char(c)) break;
            length += 1;
        }
    }

    return length;
}

/**
 * @brief Try to consume as many alphabetical, and special characters as
 * possible.
 * @param source Slice of the input string that is to be checked.
 * @return Length, in bytes, of the consumed characters.
 */
static constexpr auto validate_identifier(string_view source) -> size_t {
    Scalars scalars_iter(source);
    size_t offset = 0;

    while (true) {
        auto next = scalars_iter.next();
        if (!next.has_value() || std::iswspace(next->value) ||
            std::iswpunct(next->value) || std::iswdigit(next->value))
            break;

        offset += next->length;
    }

    return offset;
}

/**
 * @brief Split the source string into tokens.
 * @param source Input source string.
 * @return All valid tokens and errors.
 */
auto tokenize(string_view source) -> vector<Token> {
    vector<Token> tokens;
    // Index to first character of current token
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < source.length()) {
        auto rest = source.substr(start);
        char chr = source.at(start);

        size_t whitespace_len = validate_whitespace(rest);
        if (whitespace_len > 0) {
            start += whitespace_len;
            continue;
        }

        size_t number_len = validate_number(rest);
        if (number_len > 0) {
            push(TokenKind::Number, Span(start, number_len));
            start += number_len;
            continue;
        }

        size_t identifier_len = validate_identifier(rest);
        if (identifier_len > 0) {
            push(TokenKind::Identifier, Span(start, identifier_len));
            start += identifier_len;
            continue;
        }

        // Operators
        if (chr == '+')
            push(TokenKind::Plus, Span(start, 1));
        else if (chr == '-')
            push(TokenKind::Minus, Span(start, 1));
        else if (chr == '*')
            push(TokenKind::Star, Span(start, 1));
        else if (chr == '/')
            push(TokenKind::Slash, Span(start, 1));
        else
            push(TokenKind::Error, Span(start, 1));

        start += 1;
    }

    return tokens;
}
