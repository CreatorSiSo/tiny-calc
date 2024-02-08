/**
 * This project targets g++ version 12, which does not support the `format`
 * header introduced in C++ 20.
 * The functions provided in this header are thin wrappers around formatting
 * with the left-shift operator and streams.
 */

#pragma once

#include <iostream>
#include <source_location>
#include <sstream>
#include <string>

/**
 * @brief Concatenates all arguments into a new string.
 * @param ...args The arguments concatenated into one string.
 *                Each argumanet has to be convertible to `std::string`.
 */
template <typename... Args>
inline auto concat(Args... args) -> std::string {
    std::stringstream result;
    (result << ... << args);
    return result.str();
}

/**
 * @brief Concatenates and writes the provided message parts.
 *
 * Does not flush the output stream.
 *
 * @param out Output stream to write into.
 * @param ...args The arguments concatenated into one message.
 */
template <typename... Args>
inline void write(std::ostream& out, Args... args) {
    out << concat(args...);
}

/**
 * @brief Concatenates and writes the provided message parts and a newline `\\n`
 *
 * Does not flush the output stream.
 *
 * @param out Output stream to write into.
 * @param ...args The arguments concatenated into one message.
 */
template <typename... Args>
inline void writeln(std::ostream& out, Args... args) {
    out << concat(args...) << "\n";
}

/**
 * @brief Used by `panic` to capture the `std::source_location` of the caller.
 */
struct LocationArg {
    /**
     * @param arg The underlying argument.
     * @param loc Location in source code, where the argument is created.
     */
    template <class T>
    consteval LocationArg(
        const T& arg, std::source_location loc = std::source_location::current()
    )
        : arg(arg), loc(loc) {}

    std::string_view arg;
    std::source_location loc;
};

/**
 * @brief Used to quit the program in case an unrecoverable error has been
 * encountered.
 *
 * Writes the provided message parts to stderr, then aborts.
 * Flushes the output stream.
 *
 * @param ...args Message parts.
 */
template <typename... Args>
[[noreturn]]
inline void panic(LocationArg loc_arg, Args... args) {
    writeln(
        std::cerr, "panicked at ", loc_arg.loc.file_name(), ":",
        loc_arg.loc.line(), ":", loc_arg.loc.column(), ": ", loc_arg.arg,
        args...
    );
    std::cerr.flush();
    abort();
}
