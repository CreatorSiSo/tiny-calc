#pragma once

#include "common.hpp"

using Number = double;

enum class OpCode {
    Add,
    Sub,
    Mul,
    Div,
    Cos,
    Sin,
    Literal,
};

auto opcode_to_string(OpCode opcode) -> string_view;

struct Chunk {
    Chunk(vector<OpCode>&& m_opcodes, vector<Number>&& m_literals);

    auto opcodes() const -> const vector<OpCode>&;
    auto literals() const -> const vector<Number>&;

   private:
    vector<OpCode> m_opcodes;
    vector<Number> m_literals;
};
