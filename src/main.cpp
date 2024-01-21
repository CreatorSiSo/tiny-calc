#include <cctype>
#include <iostream>

#include "common.hpp"
#include "interpret.hpp"
#include "parse.hpp"
#include "tokenize.hpp"

struct Config {
    bool print_tokens;
    bool print_opcodes;
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
    else if (name == "opcodes")
        config.print_opcodes = !config.print_opcodes;
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
        .print_tokens = true,
        .print_opcodes = true,
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
            for (auto token : tokens) {
                writeln(out, "{}{}", indent, token.debug(line));
            }
        }

        auto parse_result = Parser::parse(std::move(tokens), line);
        if (!parse_result.has_value()) {
            const auto& error = parse_result.error();
            write_parse_error(error, out);
            continue;
        }
        if (config.print_opcodes) {
            const auto& chunk = *parse_result;
            writeln(out, "Op Codes:");
            for (auto opcode : chunk.op_codes()) {
                writeln(out, "{}{}", indent, static_cast<uint8_t>(opcode));
            }
            writeln(out, "Literals:");
            for (size_t i = 0; i < chunk.literals().size(); i += 1) {
                writeln(out, "{}[{}] {}", indent, i, chunk.literals()[i]);
            }
        }

        auto eval_result = interpret(std::move(*parse_result));
        // TODO eval
    }
}
