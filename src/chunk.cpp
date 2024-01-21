#include "chunk.hpp"

Chunk::Chunk(vector<OpCode>&& op_codes, vector<double>&& literals)
    : m_op_codes(std::move(op_codes)), m_literals(std::move(literals)) {}

auto Chunk::op_codes() const -> const vector<OpCode>& { return m_op_codes; }
auto Chunk::literals() const -> const vector<double>& { return m_literals; }
