#pragma once

#include <expected>

#include "chunk.hpp"
#include "common.hpp"
#include "tokenize.hpp"

// expr ::= binary | number
// binary ::= operator expr expr | number
// number ::= digit (digit | "_")* ("." (digit | "_")*)?
struct Compiler {
    /// @param tokens Enforcing move here to avoid accidental copying.
    static auto compile(vector<Token>&& tokens, string_view source)
        -> std::expected<Chunk, Report>;

   private:
    Compiler(vector<Token>&& tokens, string_view source);

    auto compile_expr() -> std::optional<Report>;

    auto next() -> const Token&;
    auto expect(TokenKind expected_kind) -> std::optional<Report>;

    const string_view m_source;
    vector<Token> m_tokens;

    size_t m_current = 0;
    vector<OpCode> m_opcodes = {};
    vector<Number> m_literals = {};
};
