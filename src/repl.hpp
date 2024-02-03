#pragma once

#include <istream>
#include <ostream>

struct Config {
    bool plain;
    bool print_tokens;
    bool print_chunks;
};

[[noreturn]]
void repl(std::istream& in, std::ostream& out, Config config);
