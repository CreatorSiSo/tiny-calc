#include <cctype>
#include <cmath>
#include <iostream>

#include "common.hpp"
#include "compile.hpp"
#include "interpret.hpp"
#include "tokenize.hpp"

struct Config {
    bool print_tokens;
    bool print_chunks;
};

static void write_help(ostream& out) {
    write(out,
          ":help     Print command help\n"
          ":quit     Exit calculator (or press CTRL+C)\n"
          ":tokens   Toggle printing the tokens\n"
          ":opcodes  Toggle printing the op-codes\n");
}

static void run_command(string_view name, Config& config, ostream& out) {
    if (name == "help")
        write_help(out);
    else if (name == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (name == "chunks")
        config.print_chunks = !config.print_chunks;
    else if (name == "quit")
        exit(0);
    else {
        writeln(out, "Error: Unkown command '{}'", name);
        writeln(out, "Note: Type :help for a list of valid commands");
    }
}

static void write_parse_error(const ParseError& error, ostream& out) {
    if (auto kind = std::get_if<ParseError::Number>(&error.kind)) {
        auto msg = *kind == ParseError::Number::Invalid
                       ? "Number literal invalid"
                       : "Number literal too large";
        writeln(out, "Error: {} at {}", msg, error.span.debug());
        // TODO Only write this note when appropriate
        writeln(out, "Note: {} is the maximum",
                std::numeric_limits<double>::max());
    } else {
        writeln(out, "Error: Unable to parse something {}", error.span.debug());
    }
}

static auto repeat(string_view value, uint32_t amount) -> string {
    string result;
    for (uint32_t i = 0; i < amount; i += 1) {
        result.append(value);
    }
    return result;
}

static void write_annotation(ostream& out, string_view input,
                             const vector<Span>& spans, string message) {
    // largest start of all spans
    size_t start = 0;
    string underlines(input.size(), ' ');
    for (Span span : spans) {
        if (span.start > start) start = span.start;
        underlines.replace(span.start, span.len,
                           repeat("^", std::max(span.len, (size_t)1)));
    }

    // TODO calculate line and column
    size_t line = 1;
    for (char c : input.substr(0, start)) {
        if (c == '\n') line += 1;
    }

    writeln(out, "Error: {}", message);
    writeln(out, "  ╭──[repl:{}:{}]", line, 0);
    writeln(out, "  │  {}", input);
    writeln(out, "──╯  {}", underlines);
}

enum class InputEnd {
    Newline,
    Eof,
};

static InputEnd get_input(istream& in, string& line) {
    line.clear();

    while (true) {
        char next = static_cast<char>(in.get());

        if (next == '\n') return InputEnd::Newline;
        if (next == std::char_traits<char>::eof()) return InputEnd::Eof;

        line.push_back(next);
    }
}

int main() {
    istream& in = std::cin;
    ostream& out = std::cout;

    constexpr auto indent = "    ";
    Config config{
        .print_tokens = false,
        .print_chunks = false,
    };
    string line;

    // Read evaluate print loop (repl)
    writeln(out, "Welcome to tiny-calc!\nType :help when you are lost =)");
    while (true) {
        write(out, ">> ");
        if (get_input(in, line) == InputEnd::Eof) {
            write(out, "CTRL+D");
            return 0;
        }
        if (line.empty()) continue;

        if (line.at(0) == ':') {
            run_command(line.substr(1), config, out);
            continue;
        }

        auto tokens = tokenize(line);
        if (config.print_tokens) {
            writeln(out, "Tokens:");
            for (const auto& token : tokens) {
                writeln(out, "{}{}", indent, token.debug(line));
            }
        }

        vector<Span> error_spans;
        for (auto& token : tokens) {
            if (token.kind == TokenKind::Error)
                error_spans.push_back(token.span);
        }
        if (!error_spans.empty()) {
            write_annotation(
                out, line, error_spans,
                error_spans.size() > 1 ? "Invalid Tokens" : "Invalid Token");
            continue;
        }

        auto parse_result = Compiler::compile(std::move(tokens), line);
        if (!parse_result.has_value()) {
            const auto& error = parse_result.error();
            write_parse_error(error, out);
            continue;
        }
        if (config.print_chunks) {
            const auto& op_codes = parse_result->op_codes();
            const auto& literals = parse_result->literals();

            writeln(out, "OpCodes:");
            for (size_t i = 0; i < op_codes.size(); i += 1) {
                writeln(out, "{}[{}] {}", indent, i,
                        op_code_to_string(op_codes[i]));
            }
            writeln(out, "Literals:");
            for (size_t i = 0; i < literals.size(); i += 1) {
                writeln(out, "{}[{}] {}", indent, i, literals[i]);
            }
        }

        auto result = interpret(std::move(*parse_result));
        writeln(out, "{}", result);
    }
}
