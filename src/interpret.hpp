#pragma once

#include "chunk.hpp"
#include "common.hpp"

/**
 * @brief Evaluates a Chunk, by execution the opcodes.
 * @param chunk The Chunk to evaluate.
 * @return Result of the calculation.
 */
auto interpret(const Chunk& chunk) -> Number;
