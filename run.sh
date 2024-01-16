clang++ src/main.cpp src/tokenize.cpp src/ast.cpp -o tiny-calc -std=c++23 -Weverything -Wno-c++98-compat-pedantic -Wno-padded && ./tiny-calc
