#pragma once

#include <istream>
#include <ostream>

struct Config {
    bool plain;
    bool print_tokens;
    bool print_chunks;
};

/**
 * @brief
 * @param in
 * @param out
 * @param config
 */
[[noreturn]]
void repl(Config config);
