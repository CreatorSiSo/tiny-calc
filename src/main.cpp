#include <iostream>
#include <string>

#include "ast.hpp"
#include "tokenize.hpp"

void print_help(std::ostream& stream) {
    stream << ":help    Print command help\n";
    stream << ":quit    Exit calculator (or press CTRL+C)\n";
    stream << ":tokens  Toggle printing the tokens\n";
    stream << ":ast     Toggle printing the ast\n";
}

struct Config {
    bool print_tokens;
    bool print_ast;
};

void run_command(std::string_view str, Config& config, std::ostream& out) {
    if (str == "help")
        print_help(out);
    else if (str == "tokens")
        config.print_tokens = !config.print_tokens;
    else if (str == "ast")
        config.print_ast = !config.print_ast;
    else
        out << "Error: Unkown command '" << str << "'\n"
            << "Note: Type :help for a list of valid commands\n";
}

int main(int argc, char* argv[]) {
    std::istream& in = std::cin;
    std::ostream& out = std::cout;
    Config config = {
        .print_tokens = false,
        .print_ast = false,
    };
    std::string line;

    out << "Welcome to tiny-calc!\nType :help when you are lost =)\n";
    while (true) {
        out << ">> ";
        std::getline(in, line);
        if (line.empty()) continue;

        if (line.at(0) == ':') {
            run_command(line.substr(1), config, out);
            continue;
        };

        auto tokens = tokenize(line);
        if (config.print_tokens) {
            out << "Tokens:\n";
            for (auto token : tokens) {
                out << "    " << token.debug(line) << "\n";
            }
        }

        auto ast =
            BinaryExpr(BinaryOp::Add, Number::alloc(0.0), Number::alloc(1.0));
        if (config.print_ast) {
            out << "Ast:\n"
                << "    " << ast << "\n";
        }

        // TODO eval
    }
}
