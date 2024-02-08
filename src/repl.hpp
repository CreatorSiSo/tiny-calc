#pragma once

/**
 * @brief Global settings for formatting and debug information.
 */
struct Config {
    bool plain;
    bool print_tokens;
    bool print_chunks;
};

/**
 * @brief Read evaluate print loop
 *
 * - Read input
 * - Tokenize input
 * - Compile tokens into chunk
 * - Interpret chunk
 * - Print solution or errors
 *
 * @param in Stdin (May be used for dependency injection)
 * @param out Stdout (May be used for dependency injection)
 * @param config Initial configuration from command line arguments
 */
[[noreturn]]
void repl(Config config);
