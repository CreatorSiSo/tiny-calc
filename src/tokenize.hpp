#include <cassert>
#include <string_view>
#include <vector>

struct Span {
    size_t start;
    size_t len;

    Span(size_t start, size_t len) : start(start), len(len) {}

    std::string debug();
    std::string_view src(std::string_view str);
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

    Token(TokenKind kind, Span span) : kind(kind), span(span) {}

    std::string debug(std::string_view src);
    std::string_view src(std::string_view src);
};

std::vector<Token> tokenize(std::string_view str);
