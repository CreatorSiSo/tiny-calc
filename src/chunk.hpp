#pragma once

#include "common.hpp"

using Number = double;

/**
 * @brief Operations/Instructions to be executed by the interpreter.
 */
enum class OpCode {
    /// pop A, pop B, push A + B
    Add,
    /// pop A, pop B, push A - B
    Sub,
    /// pop A, pop B, push A * B
    Mul,
    /// pop A, pop B, push A / B
    Div,
    /// pop A, push cos(A)
    Cos,
    /// pop A, push sin(A)
    Sin,
    /// push next literal
    Literal,
};

/**
 * @brief String name of an OpCode.
 * @param opcode The OpCode.
 * @return Name of the OpCode.
 */
auto opcode_to_string(OpCode opcode) -> string_view;

/**
 * @brief Represents valid, compiled output of an expression.
 */
struct Chunk {
    Chunk(vector<OpCode>&& m_opcodes, vector<Number>&& m_literals);

    auto opcodes() const -> const vector<OpCode>&;
    auto literals() const -> const vector<Number>&;

   private:
    vector<OpCode> m_opcodes;
    vector<Number> m_literals;
};
