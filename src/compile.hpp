#pragma once

#include <expected>

#include "chunk.hpp"
#include "common.hpp"
#include "tokenize.hpp"

/**
 * @brief Transforms tokens into a Chunk.
 */
struct Compiler {
    /**
     * @brief Validate and transform tokens into a compiled chunk.
     *
     * Only expression of this form are valid:
     * ```
     * expr ::= constant | number | unary | binary
     * binary ::= binary_op expr expr
     * binary_op ::= "+" | "-" | "*" | "/"
     * unary ::= unary_op expr
     * unary_op ::= "c" | "cos" | "s" | "sin"
     * number ::= digit (digit | "_")* ("." (digit | "_")*)?
     * constant ::= "π" | "pi"
     * ```
     *
     * @param tokens Enforcing move here to avoid accidental copying.
     * @param source Input used to generate the tokens.
     * @return Compiled chunk or an error.
     */
    static auto compile(vector<Token>&& tokens, string_view source)
        -> std::expected<Chunk, Report>;

   private:
    struct TokenStream {
        TokenStream(vector<Token>&& tokens);
        auto next() -> const Token&;
        auto expect(TokenKind expected_kind) -> std::optional<Report>;

       private:
        vector<Token> m_tokens;
        size_t m_current = 0;
    };

    Compiler(vector<Token>&& tokens, string_view source);

    auto compile_expr() -> std::optional<Report>;
    void compile_literal(Number value);
    auto compile_unary(OpCode opcode) -> std::optional<Report>;
    auto compile_binary(OpCode opcode) -> std::optional<Report>;

    const string_view m_source;
    TokenStream m_tokens;
    vector<OpCode> m_opcodes = {};
    vector<Number> m_literals = {};
};
