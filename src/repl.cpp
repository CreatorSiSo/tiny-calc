#include "repl.hpp"

#include <algorithm>
#include <iomanip>
#include <istream>
#include <iterator>
#include <ostream>

#include "compile.hpp"
#include "format.hpp"
#include "interpret.hpp"
#include "report.hpp"
#include "tokenize.hpp"

constexpr std::string_view INDENT = "    ";

constexpr std::string_view HELP =
    ":help, :?      Print command help\n"
    ":examples      Print expression examples\n"
    ":quit, :exit   Exit calculator (or press CTRL+C)\n"
    ":tokens        Toggle printing token streams\n"
    ":chunks        Toggle printing compiled chunks\n";

constexpr std::string_view EXAMPLES =
    " ╭── Addition\n"
    " │  >> + 1 2\n"
    " │  3\n"
    " │  >> + 10.2 - 0 0.2\n"
    " │  10\n"
    "─╯\n"
    " ╭── Subtraction\n"
    " │  >> - 2 1\n"
    " │  1\n"
    "─╯\n"
    " ╭── Multiplication\n"
    " │  >> * 5 0.4\n"
    " │  2\n"
    "─╯\n"
    " ╭── Division\n"
    " │  >> / 1 2\n"
    " │  0.5\n"
    "─╯\n"
    " ╭── Functions\n"
    " │  >> sin 0\n"
    " │  0\n"
    " │  >> cos 0\n"
    " │  1\n"
    "─╯\n"
    " ╭── Constants\n"
    " │  >> pi\n"
    " │  3.141592653589793\n"
    " │  >> π\n"
    " │  3.141592653589793\n"
    " │  >> * 2 pi\n"
    " │  6.283185307179586\n"
    " │  >> cos * 2 π\n"
    " │  1\n"
    "─╯\n";

/// TODO
static void run_command(
    std::ostream& out, Config& config, std::string_view name
) {
    if (name == "help" || name == "?") {
        write(out, HELP);
        return;
    }
    if (name == "examples") {
        write(out, EXAMPLES);
        return;
    }
    if (name == "tokens") {
        config.print_tokens = !config.print_tokens;
        return;
    }
    if (name == "chunks") {
        config.print_chunks = !config.print_chunks;
        return;
    }
    if (name == "quit" || name == "exit") {
        exit(0);
    }

    Report report{
        .kind = ReportKind::Error,
        .message = concat("Unkown command ':", name, "'"),
        .comments =
            {{ReportKind::Note, "Type ':help' for a list of valid commands"}}
    };
    write(out, report.format(""));
}

enum class InputEnd {
    Newline,
    Eof,
};

// TODO
static auto get_input(std::istream& in, std::string& line) -> InputEnd {
    line.clear();

    while (true) {
        char next = static_cast<char>(in.get());
        if (next == '\n') return InputEnd::Newline;
        if (next == std::char_traits<char>::eof()) return InputEnd::Eof;
        line.push_back(next);
    }
}

// TODO
static void print_tokens(
    std::ostream& out, const std::vector<Token> tokens, std::string_view source
) {
    writeln(out, "Tokens:");
    for (const auto& token : tokens) {
        writeln(
            out, INDENT, token.name(), "[", token.source(source), "] ",
            token.span.debug()
        );
    }
}

// TODO
static void print_chunk(std::ostream& out, const Chunk& chunk) {
    writeln(out, "OpCodes:");
    for (size_t i = 0; i < chunk.opcodes.size(); i += 1) {
        writeln(out, INDENT, "[", i, "] ", opcode_to_string(chunk.opcodes[i]));
    }

    writeln(out, "Literals:");
    for (size_t i = 0; i < chunk.literals.size(); i += 1) {
        writeln(out, INDENT, "[", i, "] ", chunk.literals[i]);
    }
}

[[noreturn]]
void repl(Config config) {
    std::ostream& out = std::cout;
    constexpr auto max_precision = std::numeric_limits<Number>::digits10 + 1;
    out.precision(max_precision);

    bool pretty = !config.plain;
    std::string line;
    std::string without_whitespace;

    if (pretty) {
        writeln(out, "Welcome to tiny-calc!\nType ':help' if you are lost =)");
    }

    while (true) {
        if (pretty) {
            write(out, ">> ");
            out.flush();
        }

        if (get_input(std::cin, line) == InputEnd::Eof) {
            if (pretty) {
                write(out, "CTRL+D");
            }
            exit(0);
        }

        // Skip empty lines
        without_whitespace.clear();
        for (char c : line) {
            if (!std::isspace(c)) {
                without_whitespace.push_back(c);
            }
        }
        if (without_whitespace.empty()) {
            continue;
        }

        // Execute repl command (like :help or :tokens)
        if (line.at(0) == ':') {
            run_command(out, config, line.substr(1));
            continue;
        }

        std::vector<Token> tokens = tokenize(line);
        {
            if (config.print_tokens) {
                print_tokens(out, tokens, line);
            }

            std::vector<Span> error_spans;
            for (const Token& token : tokens) {
                if (token.kind == TokenKind::Error) {
                    error_spans.push_back(token.span);
                }
            }
            if (!error_spans.empty()) {
                Report report{
                    .kind = ReportKind::Error,
                    .message = error_spans.size() > 1 ? "Invalid Tokens"
                                                      : "Invalid Token",
                    .spans = error_spans
                };
                write(out, report.format(""));
                continue;
            }
        }

        auto maybe_chunk = Compiler::compile(tokens, line);
        {
            if (!maybe_chunk.has_value()) {
                write(out, maybe_chunk.error().format(line));
                continue;
            }
            if (config.print_chunks) {
                print_chunk(out, maybe_chunk.value());
            }
        }

        Number result = interpret(maybe_chunk.value());
        writeln(out, result);
    }
}
