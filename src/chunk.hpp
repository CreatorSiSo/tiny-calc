#pragma once

#include "common.hpp"

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
    Chunk(vector<OpCode>&& m_op_codes, vector<double>&& m_literals);

    auto op_codes() const -> const vector<OpCode>&;
    auto literals() const -> const vector<double>&;

   private:
    vector<OpCode> m_op_codes;
    vector<double> m_literals;
};
