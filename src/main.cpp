#include <ranges>
#include <vector>

#include "print.hpp"
#include "repl.hpp"

auto main(int argc, char* argv[]) -> int {
    constexpr auto USAGE =
        "Usage:\n"
        "  tiny-calc [OPTIONS]\n"
        "\n"
        "Options:\n"
        "  -h, --help, -?     Print this help message\n"
        "  --plain            Only print the results of the calculation,\n"
        "                     or error messages (useful for piping)\n"
        "  --print-tokens     Print token streams\n"
        "  --print-chunks     Print compiled chunks\n";

    Config config{
        .plain = false,
        .print_tokens = false,
        .print_chunks = false,
    };

    const auto args = std::vector<std::string>(argv, argv + argc);
    for (const auto& arg : args | std::views::drop(1)) {
        if (arg == "-h" || arg == "--help" || arg == "-?") {
            writeln(
                std::cout,
                "A very minimal terminal calculator with a bit of an unusual "
                "syntax."
            );
            write(std::cout, "\n{}", USAGE);
            exit(0);
        } else if (arg == "--plain") {
            config.plain = true;
        } else if (arg == "--print-tokens") {
            config.print_tokens = true;
        } else if (arg == "--print-chunks") {
            config.print_chunks = true;
        } else {
            writeln(std::cout, "Error: Invalid argument '{}'", arg);
            write(std::cout, "\n{}", USAGE);
            exit(-1);
        }
    }

    repl(config);
}
