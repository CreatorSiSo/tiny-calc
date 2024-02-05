#include "compile.hpp"

#include <algorithm>
#include <cmath>
#include <format>
#include <ranges>

auto Compiler::compile(std::span<const Token> tokens, std::string_view source)
    -> std::expected<Chunk, Report> {
    Compiler compiler(tokens, source);

    if (const auto maybe_report = compiler.compile_expr()) {
        return std::unexpected(maybe_report.value());
    }
    if (const auto maybe_report =
            compiler.m_tokens.expect(TokenKind::EndOfInput)) {
        return std::unexpected(maybe_report.value());
    }

    // opcodes and literals are pushed back in reverse order,
    // reversing them puts them in the correct order for execution
    std::reverse(compiler.m_opcodes.begin(), compiler.m_opcodes.end());
    std::reverse(compiler.m_literals.begin(), compiler.m_literals.end());

    return Chunk(std::move(compiler.m_opcodes), std::move(compiler.m_literals));
}

Compiler::Compiler(std::span<const Token> tokens, std::string_view source)
    : m_source(source), m_tokens(TokenStream(tokens)) {}

static auto parse_number(Span span, std::string_view source)
    -> std::expected<Number, Report> {
    auto no_underscores = span.source(source) |
                          std::views::filter([](char c) { return c != '_'; });
    std::string filtered(no_underscores.begin(), no_underscores.end());

    try {
        return std::stod(filtered);
    } catch (const std::out_of_range& _) {
        std::pair<ReportKind, std::string> note = {
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

auto Compiler::compile_expr() -> std::optional<Report> {
    const Token& token = m_tokens.next();

    if (token.kind == TokenKind::Number) {
        const auto result = parse_number(token.span, m_source);
        if (result.has_value()) {
            compile_literal(result.value());
            return {};
        }
        return result.error();
    }

    if (token.kind == TokenKind::Identifier) {
        std::string_view ident = token.source(m_source);

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

    if (auto maybe_opcode = kind_to_binary_op(token.kind))
        return compile_binary(maybe_opcode.value());

    return Report(
        ReportKind::Error,
        std::format("Expected expression, found <{}>", token.name()),
        {token.span}
    );
}

void Compiler::compile_literal(Number value) {
    m_opcodes.push_back(OpCode::Load);
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

static auto end_of_input_span(std::span<const Token> tokens) -> Span {
    // Index of last character of last token
    size_t last_index = 0;
    if (!tokens.empty()) {
        Span last_span = tokens.back().span;
        last_index = last_span.start + last_span.length;
    }
    return Span(last_index, 0);
}

Compiler::TokenStream::TokenStream(std::span<const Token> tokens)
    : m_end_of_input(Token(TokenKind::EndOfInput, end_of_input_span(tokens))),
      m_tokens(tokens) {}

auto Compiler::TokenStream::next() -> const Token& {
    if (m_tokens.empty()) {
        // last token is always the end of file token (see constructor)
        return m_end_of_input;
    }

    const Token& token = *m_tokens.begin();
    m_tokens = m_tokens.subspan(1);
    return token;
}

auto Compiler::TokenStream::expect(TokenKind expected_kind)
    -> std::optional<Report> {
    auto& token = next();
    if (token.kind == expected_kind) return {};
    return Report(
        ReportKind::Error,
        std::format("Excpected <EndOfInput> found <{}>", token.name()),
        {token.span}
    );
}
