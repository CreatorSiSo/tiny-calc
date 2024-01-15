#include <iostream>
#include <string>

#include "ast.hpp"
#include "tokenize.hpp"

int main(int argc, char* argv[]) {
    std::string line;
    std::getline(std::cin, line);

    std::cout << "\nTokens:\n";

    auto tokens = tokenize(line);
    for (auto token : tokens) {
        std::cout << "    " << token.debug(line) << "\n";
    }

    std::cout << "\nAst:\n";

    auto expr =
        BinaryExpr(BinaryOp::Add, Number::alloc(0.0), Number::alloc(1.0));
    std::cout << "    " << expr << "\n";
}
