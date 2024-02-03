alias r := run
alias b := build
alias t := test

@run:
    just build && ./tiny-calc

@build:
    python3 build.py

@test:
    python3 test.py

@clean:
    rm -rf build/*

@format:
    clang-format src/*.cpp -i
