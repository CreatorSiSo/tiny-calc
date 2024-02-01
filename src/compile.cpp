#include "compile.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>

auto Compiler::compile(vector<Token>&& tokens, string_view source)
    -> std::expected<Chunk, Report> {
    Compiler compiler(std::move(tokens), source);

    if (const auto report = compiler.compile_expr())
        return std::unexpected(*report);
    if (const auto report = compiler.expect_token(TokenKind::EndOfInput))
        return std::unexpected(*report);

    // opcodes and literals are pushed back in reverse order,
    // reversing them puts them in the correct order for execution
    std::reverse(compiler.m_opcodes.begin(), compiler.m_opcodes.end());
    std::reverse(compiler.m_literals.begin(), compiler.m_literals.end());

    return Chunk(std::move(compiler.m_opcodes), std::move(compiler.m_literals));
}

Compiler::Compiler(vector<Token>&& tokens, string_view source)
    : m_source(source), m_tokens(tokens) {
    // Index of last character of last token
    size_t end = 0;
    if (!m_tokens.empty()) {
        Span last_span = m_tokens.back().span;
        end = last_span.start + last_span.len;
    }

    m_tokens.push_back(Token(TokenKind::EndOfInput, Span(end, 0)));
}

static auto parse_number(Span span, string_view source)
    -> std::expected<Number, Report> {
    auto filter_view = span.source(source) |
                       std::views::filter([](char c) { return c != '_'; });
    string filtered(filter_view.begin(), filter_view.end());

    try {
        return std::stod(filtered);
    } catch (const std::out_of_range& _) {
        std::pair<ReportKind, string> note = {
            ReportKind::Note,
            std::format("{} is the maximum", std::numeric_limits<Number>::max())
        };
        return std::unexpected(Report(
            ReportKind::Error, "Number literal too large", {span}, {note}
        ));
    } catch (const std::invalid_argument& _) {
        return std::unexpected(
            Report(ReportKind::Error, "Number literal invalid", {span})
        );
    }
}

static auto kind_to_binary_op(TokenKind kind) -> std::optional<OpCode> {
    switch (kind) {
        case TokenKind::Plus:
            return OpCode::Add;
        case TokenKind::Minus:
            return OpCode::Sub;
        case TokenKind::Star:
            return OpCode::Mul;
        case TokenKind::Slash:
            return OpCode::Div;
        default:
            return {};
    }
}

/**
 * @brief Parses tokens into an ast of expressions, mutates Parser.
 * @return Pointer to an upcasted expression tree.
 */
auto Compiler::compile_expr() -> std::optional<Report> {
    const Token& token = next_token();

    if (token.kind == TokenKind::Number) {
        const auto result = parse_number(token.span, m_source);
        if (result.has_value()) {
            compile_literal(result.value());
            return {};
        }
        return result.error();
    }

    if (token.kind == TokenKind::Identifier) {
        string_view ident = token.source(m_source);

        // Constants
        if (ident == "π" || ident == "pi") {
            compile_literal(M_PIf64);
            return {};
        }

        // Functions
        if (ident == "cos" || ident == "c")
            return compile_unary(OpCode::Cos);
        else if (ident == "sin" || ident == "s")
            return compile_unary(OpCode::Sin);

        return Report(
            ReportKind::Error,
            std::format("Unknown function or constant <{}>", ident),
            {token.span}
        );
    }

    if (auto opcode = kind_to_binary_op(token.kind))
        return compile_binary(*opcode);

    return Report(
        ReportKind::Error,
        std::format("Expected expression, found <{}>", token.name()),
        {token.span}
    );
}

void Compiler::compile_literal(Number value) {
    m_opcodes.push_back(OpCode::Literal);
    m_literals.push_back(value);
}

auto Compiler::compile_unary(OpCode opcode) -> std::optional<Report> {
    m_opcodes.push_back(opcode);
    if (auto report = compile_expr()) return report;
    return {};
}

auto Compiler::compile_binary(OpCode opcode) -> std::optional<Report> {
    m_opcodes.push_back(opcode);
    if (auto report_lhs = compile_expr()) return report_lhs;
    if (auto report_lhs = compile_expr()) return report_lhs;
    return {};
}

auto Compiler::next_token() -> const Token& {
    if (m_current < m_tokens.size()) {
        const Token& result = m_tokens.at(m_current);
        m_current += 1;
        return result;
    }

    // last token is always the end of file token (see constructor)
    return m_tokens.back();
}

auto Compiler::expect_token(TokenKind expected_kind) -> std::optional<Report> {
    auto& token = next_token();
    if (token.kind == expected_kind) return {};
    return Report(
        ReportKind::Error,
        std::format("Excpected <EndOfInput> found <{}>", token.name()),
        {token.span}
    );
}
