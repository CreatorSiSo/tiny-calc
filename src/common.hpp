#pragma once

#include <format>
#include <iostream>
#include <memory>
#include <source_location>
#include <sstream>
#include <vector>

template <typename T>
using unique_ptr = std::unique_ptr<T>;

template <typename T>
using vector = std::vector<T>;

using string = std::string;
using string_view = std::string_view;
using string_stream = std::stringstream;
using ostream = std::ostream;
using istream = std::istream;

template <typename... Args>
inline void write(ostream& out, std::format_string<Args&...> fmt,
                  Args&&... args) {
    out << std::format(fmt, args...);
}

template <typename... Args>
inline void writeln(ostream& out, std::format_string<Args&...> fmt,
                    Args&&... args) {
    out << std::format(fmt, args...) << "\n";
}

template <typename... Args>
inline void print(std::format_string<Args&...> fmt, Args&&... args) {
    write(std::cout, fmt, args...);
}

template <typename... Args>
inline void println(std::format_string<Args&...> fmt, Args&&... args) {
    writeln(std::cout, fmt, args...);
}

/// @brief Used by `panic` to capture the `std::source_location` of the caller.
template <typename... Args>
struct PanicFormat {
    template <class T>
    consteval PanicFormat(
        const T& s,
        std::source_location loc = std::source_location::current()) noexcept
        : fmt{s}, loc{loc} {}

    std::format_string<Args...> fmt;
    std::source_location loc;
};

/// @brief Prints the provided message and aborts.
/// @details See `std::format` on how formatting works.
/// @param fmt Format string to inserted arguments into.
/// @param ...args The arguments inserted into `fmt`.
template <typename... Args>
[[noreturn]] inline void panic(PanicFormat<std::type_identity_t<Args>...> fmt,
                               Args&&... args) {
    auto msg = std::format(fmt.fmt, std::forward<Args>(args)...);
    writeln(std::cerr, "panicked at {}:{}:{}: {}", fmt.loc.file_name(),
            fmt.loc.line(), fmt.loc.column(), msg);
    abort();
}
