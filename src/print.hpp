#pragma once

#include <format>
#include <iostream>
#include <source_location>

/**
 * @brief Formats and writes the provided message.
 *
 * Does not flush the output stream.
 * See `std::format` on how formatting works. (in the `format` header)
 *
 * @param out Output stream to write into.
 * @param fmt Message to inserted format arguments into.
 * @param ...args The arguments inserted into `fmt`.
 */
template <typename... Args>
inline void write(
    std::ostream& out, std::format_string<Args&...> fmt, Args&&... args
) {
    out << std::format(fmt, args...);
}

/**
 * @brief Formats and writes the provided message and a newline ("\n").
 *
 * Does not flush the output stream.
 * See `std::format` on how formatting works. (in the `format` header)
 *
 * @param out Output stream to write into.
 * @param fmt Message to inserted format arguments into.
 * @param ...args The arguments inserted into `fmt`.
 */
template <typename... Args>
inline void writeln(
    std::ostream& out, std::format_string<Args&...> fmt, Args&&... args
) {
    out << std::format(fmt, args...) << "\n";
}

/**
 * @brief Used by `panic` to capture the `std::source_location` of the caller.
 */
template <typename... Args>
struct PanicFormat {
    /**
     * @param fmt The underlying `format_string`.
     * @param loc Location in source code, where the `format_string` is created.
     */
    template <class T>
    consteval PanicFormat(
        const T& fmt, std::source_location loc = std::source_location::current()
    )
        : fmt(fmt), loc(loc) {}

    std::format_string<Args...> fmt;
    std::source_location loc;
};

/**
 * @brief Formats and writes the provided message to stderr, then aborts.
 *
 * Flushes the output stream.
 * See `std::format` on how formatting works. (in the `format` header)
 *
 * @param panic_fmt Message to inserted format arguments into.
 * @param ...args The arguments inserted into `panic_fmt`.
 */
template <typename... Args>
[[noreturn]]
inline void panic(
    PanicFormat<std::type_identity_t<Args>...> panic_fmt, Args&&... args
) {
    auto msg = std::format(panic_fmt.fmt, std::forward<Args>(args)...);
    writeln(
        std::cerr, "panicked at {}:{}:{}: {}", panic_fmt.loc.file_name(),
        panic_fmt.loc.line(), panic_fmt.loc.column(), msg
    );
    std::cerr.flush();
    abort();
}