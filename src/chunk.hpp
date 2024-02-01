#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

using Number = double;

/**
 * @brief Operations/Instructions to be executed by the interpreter.
 */
enum class OpCode : uint8_t {
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
auto opcode_to_string(OpCode opcode) -> std::string_view;

/**
 * @brief Represents valid, compiled output of an expression.
 */
struct Chunk {
    Chunk(std::vector<OpCode>&& m_opcodes, std::vector<Number>&& m_literals);

    auto opcodes() const -> const std::vector<OpCode>&;
    auto literals() const -> const std::vector<Number>&;

   private:
    std::vector<OpCode> m_opcodes;
    std::vector<Number> m_literals;
};
