#pragma once

#include <expected>
#include <span>

#include "chunk.hpp"
#include "tokenize.hpp"

/**
 * @brief Transforms tokens into a `Chunk`.
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
    static auto compile(std::span<const Token> tokens, std::string_view source)
        -> std::expected<Chunk, Report>;

   private:
    /**
     * @brief Iterator over `Token`s. Does not implement the iterator
     *        specification, as it is simply not needed.
     */
    struct TokenStream {
        TokenStream(std::span<const Token> tokens);

        /**
         * @brief Pops the next token off of the stream.
         * @return The token that was removed from the front of the stream.
         */
        auto next() -> const Token&;

        /**
         * @brief Pops the next token from the stream and generates a report
         *        if its not of the expected kind.
         * @param expected_kind What token we expect to be popped.
         * @return A report that explains which kind of token was expected.
         */
        auto expect(TokenKind expected_kind) -> std::optional<Report>;

       private:
        const Token m_end_of_input;
        std::span<const Token> m_tokens;
    };

    Compiler(std::span<const Token> tokens, std::string_view source);

    /**
     * @brief Parses an expression from the internal `TokenStream` and generates
     *        the corresponding `OpCode`s.
     * @see `Compiler::compile` on what valid expressions are
     * @return A `Report` explaining where and why compilation failed.
     */
    auto compile_expr() -> std::optional<Report>;

    /**
     * @brief Push literal value to literals and the `OpCode` for loading it.
     * @param value The literal.
     */
    void compile_literal(Number value);

    /**
     * @brief Compiles the rest of a unary expression (after the operator).
     * @param opcode
     * @return A `Report` explaining where and why compilation failed.
     */
    auto compile_unary(OpCode opcode) -> std::optional<Report>;

    /**
     * @brief Compiles the rest of a binary expression (after the operator).
     * @param opcode
     * @return A `Report` explaining where and why compilation failed.
     */
    auto compile_binary(OpCode opcode) -> std::optional<Report>;

    const std::string_view m_source;
    TokenStream m_tokens;
    std::vector<OpCode> m_opcodes = {};
    std::vector<Number> m_literals = {};
};
