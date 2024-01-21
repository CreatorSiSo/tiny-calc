#pragma once

#include <expected>

#include "chunk.hpp"
#include "common.hpp"
#include "tokenize.hpp"

struct ParseError {
    struct ExpectedFound {
        TokenKind expected;
        TokenKind found;
    };
    enum class Number {
        OutOfRange,
        Invalid,
    };

    ParseError(ExpectedFound kind, Span span);
    ParseError(Number kind, Span span);

    std::variant<ExpectedFound, Number> kind;
    Span span;
};

// expr ::= binary | number
// binary ::= operator expr expr | number
// number ::= digit (digit | "_")* ("." (digit | "_")*)?
struct Parser {
    /// @param tokens Enforcing move here to avoid accidental copying.
    static auto parse(vector<Token>&& tokens, string_view source)
        -> std::expected<Chunk, ParseError>;

   private:
    Parser(vector<Token>&& tokens, string_view source);

    auto parse_expr() -> std::optional<ParseError>;
    auto parse_number(Span span) -> std::expected<double, ParseError::Number>;

    auto expect(TokenKind kind)
        -> std::expected<Token, ParseError::ExpectedFound>;
    auto next() -> const Token&;

    size_t m_current;
    vector<Token> m_tokens;
    string_view m_source;
    vector<OpCode> m_op_codes;
    vector<double> m_literals;
};
