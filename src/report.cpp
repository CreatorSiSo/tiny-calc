#include "report.hpp"

#include <cassert>
#include <ranges>

#include "format.hpp"

Span::Span(size_t start, size_t length) : start(start), length(length) {}

auto Span::debug() const -> std::string {
    return concat(start, "..", start + length);
}

auto Span::source(std::string_view source) const -> std::string_view {
    assert(start >= 0);
    return std::string_view(source.substr(start, length));
}

/**
 * @brief Repeats `value`.
 * @param value The string to repeat.
 * @param times How often it should be repeated.
 * @return A new string.
 */
static auto repeat_string(std::string_view value, uint32_t times)
    -> std::string {
    std::string result;
    result.reserve(value.size() * times);
    for (uint32_t i = 0; i < times; i += 1) {
        result.append(value);
    }
    return result;
}

/**
 * @brief The row and column of an index in a string.
 *
 * - Row is the amount of newlines before this index.
 * - Column is the display width of the characters on the line that the index is
 *   on up the index.
 *
 * @param source The string that the index refers to.
 * @param index The index.
 * @return The first element is the row and the second one is the column.
 */
static auto row_colum(std::string_view source, size_t index)
    -> std::pair<size_t, size_t> {
    std::string_view before_index = source.substr(0, index);

    size_t row = 1;
    for (char c : before_index) {
        if (c == '\n') row += 1;
    }
    size_t line_start = before_index.rfind('\n');
    size_t column = utf8::width(before_index);
    if (line_start != std::string::npos) {
        column = utf8::width(before_index.substr(line_start));
    }

    return {row, column};
}

/**
 * @brief Write and format a string and its spans.
 *
 * Underlines spans in the string and wraps it in a block.
 *
 * @param out The stream to write into.
 * @param source The string to be underlined.
 * @param spans Spans that point to substrings in `source`.
 */
static void write_source_block(
    std::ostream& out, std::string_view source, std::span<const Span> spans
) {
    std::string underlines(utf8::width(source), ' ');

    for (Span span : spans) {
        size_t width_start = utf8::width(Span(0, span.start).source(source));
        size_t width_span = utf8::width(span.source(source));

        underlines.replace(
            width_start, span.length,
            repeat_string("^", std::max(width_span, (size_t)1))
        );
    }

    // smallest start index of all spans
    size_t min_start = source.size();
    for (Span span : spans) {
        min_start = std::min(span.start, min_start);
    }
    const auto [row, column] = row_colum(source, min_start);

    writeln(out, " ╭──[repl:", row, ":", column, "]");
    writeln(out, " │  ", source);
    writeln(out, "─╯  ", underlines);
}

/**
 * @brief Display name of a `ReportKind`.
 * @param kind The `ReportKind`
 * @return The name.
 */
static auto report_kind_to_string(ReportKind kind) -> std::string_view {
    switch (kind) {
        case ReportKind::Error:
            return "Error";
        case ReportKind::Warning:
            return "Warning";
        case ReportKind::Note:
            return "Note";
        default:
            panic(
                "Internal Error: ReportKind <{}> not covered",
                static_cast<uint8_t>(kind)
            );
    }
}

std::string Report::format(std::string_view source) const {
    std::stringstream out;

    writeln(out, report_kind_to_string(kind), ": ", message);
    if (!source.empty()) write_source_block(out, source, spans);

    for (auto& [kind, message] : comments) {
        writeln(out, report_kind_to_string(kind), ": ", message);
    }

    return out.str();
}
