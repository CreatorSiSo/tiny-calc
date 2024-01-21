#include "tokenize.hpp"

#include <cassert>
#include <cctype>

#include "common.hpp"

string Span::debug() const {
    string_stream stream;
    // stream << "Span { start: " << start << ", len: " << len << " }";
    stream << start << ".." << start + len;
    return stream.str();
}

string_view Span::source(string_view source) const {
    assert(start >= 0);
    assert(len > 0);
    return source.substr(start, len);
}

string_view Token::name() {
    switch (kind) {
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
        case TokenKind::EndOfFile:
            return "EndOfFile";
    }

    panic("unreachable");
}

string_view Token::src(string_view src) { return span.source(src); }

string Token::debug(string_view src) {
    return std::format("{}[{}] {}", name(), this->src(src), span.debug());
}

static size_t validate_whitespace(string_view str) {
    size_t len = 0;
    for (char chr : str) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

static size_t validate_number(string_view str) {
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

vector<Token> tokenize(string_view str) {
    vector<Token> tokens;
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < str.length()) {
        auto rest = str.substr(start);
        char chr = str.at(start);

        size_t number_len = validate_number(rest);
        if (number_len > 0) {
            push(TokenKind::Number, Span(start, number_len));
            start += number_len;
            continue;
        }

        size_t whitespace_len = validate_whitespace(rest);
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
