#include "interpret.hpp"

#include <cmath>

struct Stack {
    auto push(double value) -> void { m_data.push_back(value); }
    auto pop() -> double {
        double result = m_data.back();
        m_data.pop_back();
        return result;
    }

   private:
    vector<double> m_data;
};

auto interpret(Chunk&& chunk) -> double {
    Stack stack;
    size_t literal_index = 0;

    for (OpCode op_code : chunk.op_codes()) {
        switch (op_code) {
            case OpCode::Literal:
                stack.push(chunk.literals().at(literal_index));
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
        }
    }

    return stack.pop();
}