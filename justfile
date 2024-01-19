alias r := run
alias b := build

@run:
    just build
    ./tiny-calc

@build:
    python3 build.py

@clean:
    rm -rf build/*
