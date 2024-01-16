#include "tokenize.hpp"

#include <cassert>
#include <cctype>
#include <optional>

String Span::debug() {
    StringStream stream;
    // stream << "Span { start: " << start << ", len: " << len << " }";
    stream << start << ".." << start + len;
    return stream.str();
}

StringView Span::src(StringView source) {
    assert(start >= 0);
    assert(len > 0);
    return source.substr(start, len);
}

String Token::debug(StringView src) {
    StringStream stream;

    switch (kind) {
        case TokenKind::Plus:
            stream << "Plus";
            break;
        case TokenKind::Minus:
            stream << "Minus";
            break;
        case TokenKind::Star:
            stream << "Star";
            break;
        case TokenKind::Slash:
            stream << "Slash";
            break;
        case TokenKind::Number:
            stream << "Number";
            break;
        case TokenKind::Error:
            stream << "Error";
            break;
    }

    stream << '[' << span.src(src) << ']' << " " << span.debug();

    return stream.str();
}

StringView Token::src(StringView src) { return span.src(src); }

static size_t parse_whitespace(StringView str) {
    size_t len = 0;
    for (char chr : str) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

static size_t parse_number(StringView str) {
    if (!std::isdigit(str.at(0))) return 0;

    size_t len = 1;
    auto rest = str.substr(1);
    for (char chr : rest) {
        bool valid = std::isdigit(chr) || chr == '_' || chr == '.';
        if (!valid) break;
        len += 1;
    }

    return len;
}

Vector<Token> tokenize(StringView str) {
    Vector<Token> tokens;
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < str.length()) {
        auto rest = str.substr(start);
        char chr = str.at(start);

        size_t number_len = parse_number(rest);
        if (number_len > 0) {
            push(TokenKind::Number, Span(start, number_len));
            start += number_len;
            continue;
        }

        size_t whitespace_len = parse_whitespace(rest);
        if (whitespace_len > 0) {
            start += whitespace_len;
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
