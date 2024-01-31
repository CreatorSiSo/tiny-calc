#include "tokenize.hpp"

#include <cctype>

#include "utf8.hpp"

auto Token::name() const -> string_view {
    switch (kind) {
        case TokenKind::Ident:
            return "Ident";
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

auto Token::source(string_view src) const -> StringView {
    return span.source(src);
}

auto Token::debug(string_view src) const -> string {
    return std::format("{}[{}] {}", name(), this->source(src), span.debug());
}

static auto validate_whitespace(string_view str) -> size_t {
    size_t len = 0;
    for (char chr : str) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

static auto validate_number(string_view str) -> size_t {
    if (!std::isdigit(str.at(0))) return 0;

    size_t len = 1;
    auto integers = str.substr(len);
    bool has_decimal_part = false;

    auto valid_char = [](char c) { return std::isdigit(c) || c == '_'; };

    for (char c : integers) {
        if (c == '.') {
            len += 1;
            has_decimal_part = true;
            break;
        }
        if (!valid_char(c)) break;
        len += 1;
    }

    if (has_decimal_part) {
        for (char c : str.substr(len)) {
            if (!valid_char(c)) break;
            len += 1;
        }
    }

    return len;
}

static constexpr auto validate_identifier(string_view source) -> size_t {
    Chars chars(source);
    size_t offset = 0;

    while (true) {
        auto next = chars.next();
        if (!next.has_value() || std::iswspace(*next) || std::iswpunct(*next) ||
            std::iswdigit(*next))
            break;

        offset = chars.offset();
    }

    return offset;
}

auto tokenize(string_view str) -> vector<Token> {
    vector<Token> tokens;
    // First character index of current token
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < str.length()) {
        auto rest = str.substr(start);
        char chr = str.at(start);

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
            push(TokenKind::Ident, Span(start, identifier_len));
            start += identifier_len;
            continue;
        }

        // Operators
        switch (chr) {
            case '+':
                push(TokenKind::Plus, Span(start, 1));
                break;
            case '-':
                push(TokenKind::Minus, Span(start, 1));
                break;
            case '*':
                push(TokenKind::Star, Span(start, 1));
                break;
            case '/':
                push(TokenKind::Slash, Span(start, 1));
                break;
            default:
                push(TokenKind::Error, Span(start, 1));
                break;
        }
        start += 1;
    }

    return tokens;
}
