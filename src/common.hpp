#pragma once

#include <iostream>
#include <memory>
#include <sstream>
#include <vector>
#include <format>

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
