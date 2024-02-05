#include "interpret.hpp"

#include <cmath>

#include "print.hpp"

struct Stack {
    auto push(Number value) -> void { m_data.push_back(value); }
    auto pop() -> Number {
        Number result = m_data.back();
        m_data.pop_back();
        return result;
    }

   private:
    std::vector<Number> m_data;
};

auto interpret(const Chunk& chunk) -> Number {
    Stack stack;
    size_t literal_index = 0;

    for (OpCode opcode : chunk.opcodes) {
        switch (opcode) {
            case OpCode::Load:
                stack.push(chunk.literals.at(literal_index));
                literal_index += 1;
                break;
            case OpCode::Add:
                stack.push(stack.pop() + stack.pop());
                break;
            case OpCode::Sub:
                stack.push(stack.pop() - stack.pop());
                break;
            case OpCode::Mul:
                stack.push(stack.pop() * stack.pop());
                break;
            case OpCode::Div:
                stack.push(stack.pop() / stack.pop());
                break;
            case OpCode::Cos:
                stack.push(std::cos(stack.pop()));
                break;
            case OpCode::Sin:
                stack.push(std::sin(stack.pop()));
                break;
            default:
                panic(
                    "Internal Error: Unkown OpCode <{}>",
                    static_cast<uint8_t>(opcode)
                );
                break;
        }
    }

    return stack.pop();
}
