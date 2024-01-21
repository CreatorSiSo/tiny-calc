#include "chunk.hpp"

auto op_code_to_string(OpCode op_code) -> string_view {
    switch (op_code) {
        case OpCode::Add:
            return "Add";
        case OpCode::Sub:
            return "Sub";
        case OpCode::Mul:
            return "Mul";
        case OpCode::Div:
            return "Div";
        case OpCode::Cos:
            return "Cos";
        case OpCode::Literal:
            return "Literal";
        default:
            panic("OpCode[{}] not covered", static_cast<uint8_t>(op_code));
    }
}

Chunk::Chunk(vector<OpCode>&& op_codes, vector<double>&& literals)
    : m_op_codes(std::move(op_codes)), m_literals(std::move(literals)) {}

auto Chunk::op_codes() const -> const vector<OpCode>& { return m_op_codes; }
auto Chunk::literals() const -> const vector<double>& { return m_literals; }
