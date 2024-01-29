#include "report.hpp"

#include <cassert>

#include "utf8.hpp"

Span::Span(size_t start_val, size_t len_val) : start(start_val), len(len_val) {}

auto Span::debug() const -> string {
    return std::format("{}..{}", start, start + len);
}

auto Span::source(string_view source) const -> string_view {
    assert(start >= 0);
    return source.substr(start, len);
}

Report::Report(ReportKind kind, string message)
    : kind(kind), message(message) {}

Report::Report(ReportKind kind, string message, vector<Span> spans)
    : kind(kind), message(message), spans(spans) {}

Report::Report(
    ReportKind kind, string message, vector<Span> spans,
    vector<std::pair<ReportKind, string>> notes
)
    : kind(kind), message(message), spans(spans), comments(notes) {}

static auto repeat(string_view value, uint32_t amount) -> string {
    string result;
    for (uint32_t i = 0; i < amount; i += 1) {
        result.append(value);
    }
    return result;
}

auto Report::kind_to_string(ReportKind kind) -> string_view {
    switch (kind) {
        case ReportKind::Error:
            return "Error";
        case ReportKind::Note:
            return "Note";
        default:
            panic("Case not covered");
    }
}

void write_report(ostream& out, string_view input, Report report) {
    // largest start of all spans
    size_t smallest_start = 0;
    string underlines = string(input.size(), ' ');

    for (Span span : report.spans) {
        if (span.start < smallest_start) smallest_start = span.start;

        size_t width_start =
            utf8_amount_scalars(Span(0, span.start).source(input));
        size_t width_span = utf8_amount_scalars(span.source(input));

        underlines.replace(
            width_start, span.len, repeat("^", std::max(width_span, (size_t)1))
        );
    }

    size_t line = 1;
    for (char c : input.substr(0, smallest_start)) {
        if (c == '\n') line += 1;
    }
    // TODO calculate column

    writeln(out, "{}: {}", Report::kind_to_string(report.kind), report.message);
    writeln(out, " ╭──[repl:{}:{}]", line, 0);
    writeln(out, " │  {}", input);
    writeln(out, "─╯  {}", underlines);

    for (auto& comment : report.comments) {
        writeln(
            out, "{}: {}", Report::kind_to_string(comment.first), comment.second
        );
    }
}
