#pragma once

#include "common.hpp"

enum class OpCode {
    Add,
    Sub,
    Mul,
    Div,
    Cos,
    Literal,
};

struct Chunk {
    Chunk(vector<OpCode>&& m_op_codes, vector<double>&& m_literals);

    auto op_codes() const -> const vector<OpCode>&;
    auto literals() const -> const vector<double>&;

   private:
    vector<OpCode> m_op_codes;
    vector<double> m_literals;
};
