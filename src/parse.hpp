#pragma once

#include <expected>

#include "ast.hpp"
#include "common.hpp"
#include "tokenize.hpp"

struct ParseError {
    TokenKind expected;
    Token found;
};

// expr ::= binary | number
// binary ::= operator expr expr | number
// number ::= digit (digit | "_")* ("." (digit | "_")*)?
struct Parser {
    Parser(vector<Token> m_tokens, string_view source);

    auto parse_expr() -> unique_ptr<Expr>;
    // auto parse_number() -> Number;

    auto expect(TokenKind kind) -> std::expected<Token, ParseError>;
    auto next() -> const Token&;

   private:
    size_t m_current;
    vector<Token> m_tokens;
    string_view m_source;
};
