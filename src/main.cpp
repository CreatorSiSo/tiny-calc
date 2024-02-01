#include <cctype>
#include <cmath>
#include <locale>

#include "compile.hpp"
#include "interpret.hpp"
#include "print.hpp"
#include "report.hpp"
#include "tokenize.hpp"

struct Config {
    bool print_tokens;
    bool print_chunks;
};

constexpr const char* INDENT = "    ";

static void write_help(std::ostream& out) {
    write(
        out,
        ":help, :?      Print command help\n"
        ":examples      Print expression examples\n"
        ":quit, :exit   Exit calculator (or press CTRL+C)\n"
        ":tokens        Toggle printing token streams\n"
        ":chunks        Toggle printing compiled chunks\n"
    );
}

static void write_examples(std::ostream& out) {
    write(
        out,
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
        "─╯\n"
    );
}

static void run_command(
    std::string_view name, Config& config, std::ostream& out
) {
    if (name == "help")
        write_help(out);
    else if (name == "examples")
        write_examples(out);
    else if (name == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (name == "chunks")
        config.print_chunks = !config.print_chunks;
    else if (name == "quit" || name == "exit")
        exit(0);
    else {
        writeln(out, "Error: Unkown command <:{}>", name);
        writeln(out, "Note: Type <:help> for a list of valid commands");
    }
}

enum class InputEnd {
    Newline,
    Eof,
};

static auto get_input(std::istream& in, std::string& line) -> InputEnd {
    line.clear();

    while (true) {
        char next = static_cast<char>(in.get());

        if (next == '\n') return InputEnd::Newline;
        if (next == std::char_traits<char>::eof()) return InputEnd::Eof;

        line.push_back(next);
    }
}

static void print_tokens(
    std::ostream& out, const std::vector<Token> tokens, std::string_view source
) {
    writeln(out, "Tokens:");
    for (const auto& token : tokens) {
        writeln(
            out, "{}{}[{}] {}", INDENT, token.name(), token.source(source),
            token.span.debug()
        );
    }
}

static void print_chunk(std::ostream& out, const Chunk chunk) {
    const auto& opcodes = chunk.opcodes();
    const auto& literals = chunk.literals();

    writeln(out, "OpCodes:");
    for (size_t i = 0; i < opcodes.size(); i += 1) {
        writeln(out, "{}[{}] {}", INDENT, i, opcode_to_string(opcodes[i]));
    }
    writeln(out, "Literals:");
    for (size_t i = 0; i < literals.size(); i += 1) {
        writeln(out, "{}[{}] {}", INDENT, i, literals[i]);
    }
}

int main() {
    std::istream& in = std::cin;
    std::ostream& out = std::cout;

    Config config{
        .print_tokens = false,
        .print_chunks = false,
    };
    std::string line;

    // Read evaluate print loop (repl)
    writeln(out, "Welcome to tiny-calc!\nType :help when you are lost =)");
    while (true) {
        write(out, ">> ");
        auto input_end = get_input(in, line);
        if (input_end == InputEnd::Eof) {
            write(out, "CTRL+D");
            return 0;
        }
        if (line.empty()) continue;

        // Execute repl command (like :help or :tokens)
        if (line.at(0) == ':') {
            run_command(line.substr(1), config, out);
            continue;
        }

        auto tokens = tokenize(line);
        {
            if (config.print_tokens) print_tokens(out, tokens, line);

            std::vector<Span> error_spans;
            for (auto& token : tokens) {
                if (token.kind == TokenKind::Error)
                    error_spans.push_back(token.span);
            }
            if (!error_spans.empty()) {
                std::string message =
                    error_spans.size() > 1 ? "Invalid Tokens" : "Invalid Token";
                write_report(
                    out, line, Report(ReportKind::Error, message, error_spans)
                );
                continue;
            }
        }

        auto chunk = Compiler::compile(tokens, line);
        {
            if (!chunk.has_value()) {
                const auto& report = chunk.error();
                write_report(out, line, report);
                continue;
            }
            if (config.print_chunks) print_chunk(out, *chunk);
        }

        auto result = interpret(*chunk);
        writeln(out, "{}", result);
    }
}
