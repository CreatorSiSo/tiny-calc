#include <cctype>
#include <iostream>

#include "ast.hpp"
#include "common.hpp"
#include "interpret.hpp"
#include "parse.hpp"
#include "tokenize.hpp"

struct Config {
    bool print_tokens;
    bool print_ast;
};

static void write_help(ostream& out) {
    write(out,
          ":help    Print command help\n"
          ":quit    Exit calculator (or press CTRL+C)\n"
          ":tokens  Toggle printing the tokens\n"
          ":ast     Toggle printing the ast\n");
}

static void run_command(string_view name, Config& config, ostream& out) {
    if (name == "help")
        write_help(out);
    else if (name == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (name == "ast")
        config.print_ast = !config.print_ast;
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
        .print_ast = true,
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

        auto parse_result = Parser(std::move(tokens), line).parse_expr();
        if (!parse_result.has_value()) {
            const auto& error = parse_result.error();
            write_parse_error(error, out);
            continue;
        }
        if (config.print_ast) {
            writeln(out, "Ast:");
            writeln(out, "{}{}", indent, **parse_result);
        }

        auto eval_result = eval_expr(std::move(*parse_result));
        // TODO eval
    }
}
