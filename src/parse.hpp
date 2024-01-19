#pragma once

#include <span>

#include "ast.hpp"
#include "common.hpp"
#include "tokenize.hpp"

Box<Expr> parse(StringView src, std::span<Token> tokens);
