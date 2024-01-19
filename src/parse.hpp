#pragma once

#include <span>

#include "ast.hpp"
#include "common.hpp"
#include "tokenize.hpp"

unique_ptr<Expr> parse(string_view src, std::span<Token> tokens);
