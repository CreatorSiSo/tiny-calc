#pragma once

#include <format>
#include <iostream>
#include <memory>
#include <source_location>
#include <vector>

template <typename T>
using vector = std::vector<T>;

using string = std::string;
using string_view = std::string_view;
using ostream = std::ostream;
using istream = std::istream;

template <typename... Args>
inline void write(
    ostream& out, std::format_string<Args&...> fmt, Args&&... args
) {
    out << std::format(fmt, args...);
}

template <typename... Args>
inline void writeln(
    ostream& out, std::format_string<Args&...> fmt, Args&&... args
) {
    out << std::format(fmt, args...) << "\n";
}

/// @brief Used by `panic` to capture the `std::source_location` of the caller.
template <typename... Args>
struct PanicFormat {
    /**
     * @param s The underlying `format_string`.
     * @param loc Location in source code, where the `format_string` is created.
     */
    template <class T>
    consteval PanicFormat(
        const T& s, std::source_location loc = std::source_location::current()
    )
        : fmt(s), loc(loc) {}

    std::format_string<Args...> fmt;
    std::source_location loc;
};

/// @brief Prints the provided message and aborts.
///
/// See `std::format` on how formatting works.
///
/// @param panic_fmt `format_string` to inserted arguments into.
/// @param ...args The arguments inserted into `fmt`.
template <typename... Args>
[[noreturn]] inline void panic(
    PanicFormat<std::type_identity_t<Args>...> panic_fmt, Args&&... args
) {
    auto msg = std::format(panic_fmt.fmt, std::forward<Args>(args)...);
    writeln(
        std::cerr, "panicked at {}:{}:{}: {}", panic_fmt.loc.file_name(),
        panic_fmt.loc.line(), panic_fmt.loc.column(), msg
    );
    abort();
}
