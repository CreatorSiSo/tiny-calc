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

auto op_code_to_string(OpCode op_code) -> string_view;

struct Chunk {
    Chunk(vector<OpCode>&& m_op_codes, vector<Number>&& m_literals);

    auto op_codes() const -> const vector<OpCode>&;
    auto literals() const -> const vector<Number>&;

   private:
    vector<OpCode> m_op_codes;
    vector<Number> m_literals;
};
