#include <iostream>
#include <string>

#include "tokenize.hpp"

int main(int argc, char* argv[]) {
    std::string line;
    std::getline(std::cin, line);

    auto tokens = tokenize(line);
    for (auto token : tokens) {
        std::cout << token.debug(line) << "\n";
    }
}
