#include "chunk.hpp"

#include "common.hpp"

auto opcode_to_string(OpCode opcode) -> std::string_view {
    switch (opcode) {
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
        case OpCode::Sin:
            return "Sin";
        case OpCode::Literal:
            return "Literal";
        default:
            panic("OpCode[{}] not covered", static_cast<uint8_t>(opcode));
    }
}

Chunk::Chunk(std::vector<OpCode>&& opcodes, std::vector<Number>&& literals)
    : m_opcodes(std::move(opcodes)), m_literals(std::move(literals)) {}

auto Chunk::opcodes() const -> const std::vector<OpCode>& { return m_opcodes; }
auto Chunk::literals() const -> const std::vector<Number>& {
    return m_literals;
}
