#include "chunk.hpp"

#include "print.hpp"

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
        case OpCode::Load:
            return "Literal";
        default:
            panic(
                "Internal Error: OpCode <",
                std::to_string(static_cast<uint8_t>(opcode)),
                "> not covered"
            );
    }
}

Chunk::Chunk(std::vector<OpCode>&& opcodes, std::vector<Number>&& literals)
    : opcodes(std::move(opcodes)), literals(std::move(literals)) {}
