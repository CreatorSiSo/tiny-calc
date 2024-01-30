#include "compile.hpp"

#include <algorithm>

auto Compiler::compile(vector<Token>&& tokens, string_view source)
    -> std::expected<Chunk, Report> {
    Compiler compiler(std::move(tokens), source);
    if (const auto report = compiler.compile_expr(); report.has_value()) {
        return std::unexpected(*report);
    }
    if (auto& token = compiler.next(); token.kind != TokenKind::EndOfInput) {
        return std::unexpected(Report(
            ReportKind::Error,
            std::format("Excpected <EndOfInput> found <{}>", token.name()),
            {token.span}
        ));
    }
    std::reverse(compiler.m_op_codes.begin(), compiler.m_op_codes.end());
    std::reverse(compiler.m_literals.begin(), compiler.m_literals.end());
    return Chunk(
        std::move(compiler.m_op_codes), std::move(compiler.m_literals)
    );
}

Compiler::Compiler(vector<Token>&& tokens, string_view source)
    : m_current(0), m_tokens(tokens), m_source(source) {
    // Insert end of file token
    size_t end = 0;
    if (!m_tokens.empty()) {
        Span last_span = m_tokens.back().span;
        end = last_span.start + last_span.len;
    }
    m_tokens.push_back(Token(TokenKind::EndOfInput, Span(end, 0)));
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

static auto parse_number(Span span, string_view source)
    -> std::expected<double, Report> {
    string number_string;
    for (char chr : span.source(source)) {
        if (chr != '_') number_string.push_back(chr);
    }
    try {
        return std::stod(number_string);
    } catch (const std::out_of_range& e) {
        auto note = std::make_pair(
            ReportKind::Note,
            std::format("{} is the maximum", std::numeric_limits<double>::max())
        );
        return std::unexpected(Report(
            ReportKind::Error, "Number literal too large", {span}, {note}
        ));
    } catch (const std::invalid_argument& e) {
        return std::unexpected(
            Report(ReportKind::Error, "Number literal invalid", {span})
        );
    }
}

/// @brief Parses tokens into an ast of expressions, mutates Parser.
/// @return Pointer to an upcasted expression tree.
auto Compiler::compile_expr() -> std::optional<Report> {
    const Token& token = next();

    if (token.kind == TokenKind::Number) {
        const auto result = parse_number(token.span, m_source);
        if (result.has_value()) {
            m_op_codes.push_back(OpCode::Literal);
            m_literals.push_back(*result);
            return {};
        }
        return result.error();
    }

    if (token.kind == TokenKind::Ident) {
        StringView ident = token.source(m_source);
        if (ident == "cos" || ident == "c") {
            m_op_codes.push_back(OpCode::Cos);
        } else {
            return Report(
                ReportKind::Error, std::format("Unknown function <{}>", ident),
                {token.span}
            );
        }

        auto maybe_error = compile_expr();
        if (maybe_error.has_value()) return maybe_error;
        return {};
    }

    if (auto op_code = kind_to_binary_op(token.kind); op_code.has_value()) {
        m_op_codes.push_back(*op_code);

        if (auto err_lhs = compile_expr(); err_lhs.has_value()) return err_lhs;
        if (auto err_rhs = compile_expr(); err_rhs.has_value()) return err_rhs;

        return {};
    }

    return Report(
        ReportKind::Error,
        std::format("Expected expression, found <{}>", token.name()),
        {token.span}
    );
}

auto Compiler::next() -> const Token& {
    if (m_current < m_tokens.size()) {
        const Token& result = m_tokens.at(m_current);
        m_current += 1;
        return result;
    } else {
        // last token is always end of file token
        return m_tokens.back();
    }
}
