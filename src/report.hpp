#pragma once

#include "common.hpp"

struct Span {
    size_t start;
    size_t len;

    Span(size_t start_val, size_t len_val);

    auto debug() const -> string;
    auto source(string_view str) const -> string_view;
};

struct Report {
    Report(string message, vector<Span> spans)
        : message(message), spans(spans) {}
    string message;
    vector<Span> spans;
};

void write_report(ostream& out, string_view input, Report report);
