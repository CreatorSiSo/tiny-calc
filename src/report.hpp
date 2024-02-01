#pragma once

#include <string>
#include <vector>

#include "utf8.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val);

    auto debug() const -> std::string;
    auto source(std::string_view str) const -> std::string_view;
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
