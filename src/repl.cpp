#include "repl.hpp"

#include <algorithm>
#include <iterator>
#include <sstream>

#include "compile.hpp"
#include "interpret.hpp"
#include "print.hpp"
#include "report.hpp"
#include "tokenize.hpp"

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
    std::ostream& out, Config& config, std::string_view name
) {
    if (name == "help" || name == "?")
        write_help(out);
    else if (name == "examples")
        write_examples(out);
    else if (name == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (name == "chunks")
        config.print_chunks = !config.print_chunks;
    else if (name == "quit" || name == "exit")
        exit(0);
    else
        Report(
            ReportKind::Error, std::format("Unkown command <:{}>", name), {},
            {{ReportKind::Note, "Type <:help> for a list of valid commands"}}
        ).write(out, "");
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
    writeln(out, "OpCodes:");
    for (size_t i = 0; i < chunk.opcodes.size(); i += 1) {
        writeln(
            out, "{}[{}] {}", INDENT, i, opcode_to_string(chunk.opcodes[i])
        );
    }

    writeln(out, "Literals:");
    for (size_t i = 0; i < chunk.literals.size(); i += 1) {
        writeln(out, "{}[{}] {}", INDENT, i, chunk.literals[i]);
    }
}

/**
 * @brief Read evaluate print loop
 *
 * - Read input
 * - Tokenize input
 * - Compile tokens into chunk
 * - Interpret chunk
 * - Print solution or errors
 *
 * @param in Stdin (May be used for dependency injection)
 * @param out Stdout (May be used for dependency injection)
 * @param config Initial configuration from command line arguments
 */
[[noreturn]]
void repl(Config config) {
    bool pretty = !config.plain;
    std::string line;
    std::string without_whitespace;
    std::ostream& out = std::cout;

    if (pretty) {
        writeln(out, "Welcome to tiny-calc!\nType :help if you are lost =)");
    }

    while (true) {
        if (pretty) {
            write(out, ">> ");
        }

        if (get_input(std::cin, line) == InputEnd::Eof) {
            if (pretty) write(out, "CTRL+D");
            exit(0);
        }

        without_whitespace.clear();
        for (char c : line) {
            if (!std::isspace(c)) without_whitespace.push_back(c);
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
            for (auto& token : tokens) {
                if (token.kind == TokenKind::Error) {
                    error_spans.push_back(token.span);
                }
            }
            if (!error_spans.empty()) {
                Report(
                    ReportKind::Error,
                    error_spans.size() > 1 ? "Invalid Tokens" : "Invalid Token",
                    error_spans
                )
                    .write(out, line);
                continue;
            }
        }

        auto maybe_chunk = Compiler::compile(tokens, line);
        {
            if (!maybe_chunk.has_value()) {
                maybe_chunk.error().write(out, line);
                continue;
            }
            if (config.print_chunks) {
                print_chunk(out, maybe_chunk.value());
            }
        }

        Number result = interpret(maybe_chunk.value());
        writeln(out, "{}", result);
    }
}
