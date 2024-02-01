#pragma once

#include <string>
#include <vector>

#include "utf8.hpp"

/**
 * @brief Refers to a range of bytes in the input string,
 *        usually associated with an error.
 */
struct Span {
    size_t start;
    size_t length;

    Span(size_t start, size_t length);

    /**
     * @brief Formats the internal data of the span for debug printing.
     */
    auto debug() const -> std::string;

    /**
     * @brief Substring that the span refers to.
     * @param string String that this span refers to.
     * @pre Span references a slice that is a valid substring of `string`.
     *      `span.start <= string.size()` and `span.length <= string.size()`
     *      always must be true
     * @return Substring of `string`.
     */
    auto source(std::string_view string) const -> std::string_view;
};

enum class ReportKind {
    Error,
    Note,
};

struct Report {
    Report(ReportKind kind, std::string message);
    Report(ReportKind kind, std::string message, std::vector<Span> spans);
    Report(
        ReportKind kind, std::string message, std::vector<Span> spans,
        std::vector<std::pair<ReportKind, std::string>> notes
    );

    static auto kind_to_string(ReportKind kind) -> std::string_view;

    const ReportKind kind;
    const std::string message;
    const std::vector<Span> spans;
    const std::vector<std::pair<ReportKind, std::string>> comments;
};

void write_report(std::ostream& out, std::string_view input, Report report);
