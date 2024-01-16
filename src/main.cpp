#include <cctype>
#include <iostream>

#include "ast.hpp"
#include "common.hpp"
#include "tokenize.hpp"

static void print_help(Ostream& out) {
    out << ":help    Print command help\n"
        << ":quit    Exit calculator (or press CTRL+C)\n"
        << ":tokens  Toggle printing the tokens\n"
        << ":ast     Toggle printing the ast\n";
}

struct Config {
    bool print_tokens;
    bool print_ast;
};

static void run_command(StringView name, Config& config, Ostream& out) {
    if (name == "help")
        print_help(out);
    else if (name == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (name == "ast")
        config.print_ast = !config.print_ast;
    else if (name == "quit")
        exit(0);
    else
        out << "Error: Unkown command '" << name << "'\n"
            << "Note: Type :help for a list of valid commands\n";
}

enum class InputEnd {
    Newline,
    Eof,
};
static InputEnd get_input(Istream& in, String& line) {
    line.clear();

    while (true) {
        char next = static_cast<char>(in.get());

        if (next == '\n') return InputEnd::Newline;
        if (next == std::char_traits<char>::eof()) return InputEnd::Eof;

        line.push_back(next);
    }
}

int main() {
    Istream& in = std::cin;
    Ostream& out = std::cout;

    Config config{
        .print_tokens = true,
        .print_ast = true,
    };
    String line;

    out << "Welcome to tiny-calc!\nType :help when you are lost =)\n";
    while (true) {
        out << ">> ";
        if (get_input(in, line) == InputEnd::Eof) {
            out << "CTRL+D";
            return 0;
        }
        if (line.empty()) continue;

        if (line.at(0) == ':') {
            run_command(line.substr(1), config, out);
            continue;
        }

        auto tokens = tokenize(line);
        if (config.print_tokens) {
            out << "Tokens:\n";
            for (auto token : tokens) {
                out << "    " << token.debug(line) << "\n";
            }
        }

        auto ast =
            BinaryExpr(BinaryOp::Add,
                       BinaryExpr::alloc(BinaryOp::Cos, Number::alloc(0.0),
                                         Number::alloc(8237364536.2304823084)),
                       Number::alloc(1.0));
        if (config.print_ast) {
            out << "Ast:\n"
                << "    " << ast << "\n";
        }

        // TODO eval
    }
}
