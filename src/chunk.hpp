#pragma once

#include <cstdint>
#include <string_view>
#include <vector>

/**
 * @brief Number type used when evaluating the expression.
 *
 * Not wrapped in a class to allow easy access to the underlying data
 * (needed to get the max value and max precision)
 */
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
    Load,
};

/**
 * @brief String name of an OpCode.
 * @param opcode The OpCode.
 * @return Name of the OpCode.
 */
auto opcode_to_string(OpCode opcode) -> std::string_view;

/**
 * @brief Represents a validated expression, compiled to opcodes and
 *        literals.
 */
struct Chunk {
    Chunk(std::vector<OpCode>&& m_opcodes, std::vector<Number>&& m_literals);

    const std::vector<OpCode> opcodes;
    const std::vector<Number> literals;
};
