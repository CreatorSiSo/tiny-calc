#include "report.hpp"

#include <cassert>
#include <format>

#include "print.hpp"

Span::Span(size_t start, size_t length) : start(start), length(length) {}

auto Span::debug() const -> std::string {
    return std::format("{}..{}", start, start + length);
}

auto Span::source(std::string_view source) const -> std::string_view {
    assert(start >= 0);
    return std::string_view(source.substr(start, length));
}

Report::Report(ReportKind kind, std::string message)
    : kind(kind), message(message) {}

Report::Report(ReportKind kind, std::string message, std::vector<Span> spans)
    : kind(kind), message(message), spans(spans) {}

Report::Report(
    ReportKind kind, std::string message, std::vector<Span> spans,
    std::vector<std::pair<ReportKind, std::string>> comments
)
    : kind(kind), message(message), spans(spans), comments(comments) {}

static auto repeat_string(std::string_view value, uint32_t times)
    -> std::string {
    std::string result;
    result.reserve(value.size() * times);
    for (uint32_t i = 0; i < times; i += 1) {
        result.append(value);
    }
    return result;
}

auto Report::kind_to_string(ReportKind kind) -> std::string_view {
    switch (kind) {
        case ReportKind::Error:
            return "Error";
        case ReportKind::Note:
            return "Note";
        default:
            panic("Case not covered");
    }
}

void write_report(std::ostream& out, std::string_view input, Report report) {
    // largest start of all spans
    size_t smallest_start = 0;
    std::string underlines(utf8_width(input), ' ');

    for (Span span : report.spans) {
        auto [start, length] = span;
        if (start < smallest_start) smallest_start = start;

        size_t width_start = utf8_width(Span(0, start).source(input));
        size_t width_span = utf8_width(span.source(input));

        underlines.replace(
            width_start, length,
            repeat_string("^", std::max(width_span, (size_t)1))
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

    for (auto& [kind, message] : report.comments) {
        writeln(out, "{}: {}", Report::kind_to_string(kind), message);
    }
}
