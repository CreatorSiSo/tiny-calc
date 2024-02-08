#pragma once

#include "chunk.hpp"

/**
 * @brief Evaluates a Chunk, by executing the opcodes.
 * @param chunk The Chunk to evaluate.
 * @return Result of the calculation.
 */
auto interpret(const Chunk& chunk) -> Number;
