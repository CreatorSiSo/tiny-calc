#pragma once

#include <expected>

#include "chunk.hpp"
#include "common.hpp"
#include "tokenize.hpp"

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
auto compile(std::span<const Token> tokens, string_view source)
    -> std::expected<Chunk, Report>;
