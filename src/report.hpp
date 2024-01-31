#pragma once

#include "common.hpp"
#include "utf8.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val);

    auto debug() const -> string;
    auto source(string_view str) const -> string_view;
};

enum class ReportKind {
    Error,
    Note,
};

struct Report {
    Report(ReportKind kind, string message);
    Report(ReportKind kind, string message, vector<Span> spans);
    Report(
        ReportKind kind, string message, vector<Span> spans,
        vector<std::pair<ReportKind, string>> notes
    );

    static auto kind_to_string(ReportKind kind) -> string_view;

    const ReportKind kind;
    const string message;
    const vector<Span> spans;
    const vector<std::pair<ReportKind, string>> comments;
};

void write_report(ostream& out, string_view input, Report report);
