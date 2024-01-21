#include <cctype>
#include <cmath>

#include "common.hpp"
#include "compile.hpp"
#include "interpret.hpp"
#include "report.hpp"
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
        auto input_end = get_input(in, line);
        if (input_end == InputEnd::Eof) {
            write(out, "CTRL+D");
            return 0;
        }
        if (line.empty()) continue;

        if (line.at(0) == ':') {
            run_command(line.substr(1), config, out);
            continue;
        }

        auto tokens = tokenize(line);
        {
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
                string message =
                    error_spans.size() > 1 ? "Invalid Tokens" : "Invalid Token";
                write_report(out, line,
                             Report(ReportKind::Error, message, error_spans));
                continue;
            }
        }

        auto maybe_chunk = Compiler::compile(std::move(tokens), line);
        {
            if (!maybe_chunk.has_value()) {
                const auto& report = maybe_chunk.error();
                write_report(out, line, report);
                continue;
            }
            if (config.print_chunks) {
                const auto& op_codes = maybe_chunk->op_codes();
                const auto& literals = maybe_chunk->literals();

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
        }

        auto result = interpret(std::move(*maybe_chunk));
        writeln(out, "{}", result);
    }
}
