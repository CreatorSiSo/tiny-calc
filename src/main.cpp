#include <cctype>
#include <cmath>
#include <locale>

#include "common.hpp"
#include "compile.hpp"
#include "interpret.hpp"
#include "report.hpp"
#include "tokenize.hpp"

struct Config {
    bool print_tokens;
    bool print_chunks;
};

constexpr const char* INDENT = "    ";

static void write_help(ostream& out) {
    write(
        out,
        ":help     Print command help\n"
        ":quit     Exit calculator (or press CTRL+C)\n"
        ":tokens   Toggle printing token streams\n"
        ":chunks   Toggle printing compiled chunks\n"
    );
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

static void print_tokens(
    ostream& out, const vector<Token> tokens, string_view source
) {
    writeln(out, "Tokens:");
    for (const auto& token : tokens) {
        writeln(
            out, "{}{}[{}] {}", INDENT, token.name(), token.source(source),
            token.span.debug()
        );
    }
}

static void print_chunk(ostream& out, const Chunk chunk) {
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
    istream& in = std::cin;
    ostream& out = std::cout;

    Config config{
        .print_tokens = false,
        .print_chunks = false,
    };
    string line;

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

            vector<Span> error_spans;
            for (auto& token : tokens) {
                if (token.kind == TokenKind::Error)
                    error_spans.push_back(token.span);
            }
            if (!error_spans.empty()) {
                string message =
                    error_spans.size() > 1 ? "Invalid Tokens" : "Invalid Token";
                write_report(
                    out, line, Report(ReportKind::Error, message, error_spans)
                );
                continue;
            }
        }

        auto chunk = compile(tokens, line);
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
