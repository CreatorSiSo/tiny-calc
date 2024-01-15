#include "tokenize.h"

#include <cctype>
#include <optional>
#include <string>

std::string_view Span::src(std::string_view source) {
    assert(start >= 0);
    assert(len > 0);
    return source.substr(start, len);
}

std::string Token::debug(std::string_view src) {
    std::string result;

    switch (kind) {
        case TokenKind::Plus:
            result.append("Plus");
            break;
        case TokenKind::Minus:
            result.append("Minus");
            break;
        case TokenKind::Star:
            result.append("Star");
            break;
        case TokenKind::Slash:
            result.append("Slash");
            break;
        case TokenKind::Number:
            result.append("Number");
            break;
        case TokenKind::Error:
            result.append("Error");
            break;
    }

    result.push_back('[');
    result.append(span.src(src));
    result.push_back(']');

    return result;
}

std::string_view Token::display(std::string_view src) { return span.src(src); }

size_t parse_whitespace(std::string_view str) {
    size_t len = 0;
    for (char chr : str) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

size_t parse_number(std::string_view str) {
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

std::vector<Token> tokenize(std::string_view str) {
    std::vector<Token> tokens;
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < str.length()) {
        char chr = str.at(start);

        size_t number_len = parse_number(str);
        if (number_len > 0) {
            push(TokenKind::Number, Span(start, number_len));
            start += number_len;
            continue;
        }

        size_t whitespace_len = parse_whitespace(str);
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
