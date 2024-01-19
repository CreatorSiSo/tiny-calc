#pragma once

#include <expected>

#include "ast.hpp"
#include "common.hpp"
#include "tokenize.hpp"

struct ExpectedFoundError {
    TokenKind expected;
    Token found;
};
struct ParseNumberError {};
using ParseExprError = std::variant<ExpectedFoundError, ParseNumberError>;

// expr ::= binary | number
// binary ::= operator expr expr | number
// number ::= digit (digit | "_")* ("." (digit | "_")*)?
struct Parser {
    /// @param tokens Enforcing move here to avoid accidental copying.
    Parser(vector<Token>&& tokens, string_view source);

    auto parse_expr() -> std::expected<unique_ptr<Expr>, ParseExprError>;
    auto parse_number(Span span) -> std::expected<double, ParseNumberError>;

    auto expect(TokenKind kind) -> std::expected<Token, ExpectedFoundError>;
    auto next() -> const Token&;

   private:
    size_t m_current;
    vector<Token> m_tokens;
    string_view m_source;
};
