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
     * @return The formatted span.
     */
    auto debug() const -> std::string;

    /**
     * @brief Substring that the span points to.
     * @param string String that this span refers to.
     * @pre Span points to a slice that is a valid substring of `string`.
     *      `span.start <= string.size()` and `span.length <= string.size()`
     *      must always be true.
     * @return Substring of `string`.
     */
    auto source(std::string_view string) const -> std::string_view;
};

/**
 * @brief The category of a `Report`
 */
enum class ReportKind {
    Error,
    Warning,
    Note,
};

/**
 * @brief A message, often associated with location in the input string.
 */
struct Report {
    const ReportKind kind;
    const std::string message;
    const std::vector<Span> spans;
    const std::vector<std::pair<ReportKind, std::string>> comments;

    /**
     * @brief Formats the report message and underlines the part of input that
     *        its span it points to.
     * @param source The input that used to generate this `Span`.
     * @pre @see `Span::source`
     */
    std::string format(std::string_view source) const;
};
