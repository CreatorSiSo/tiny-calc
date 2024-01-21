#include "compile.hpp"

auto Compiler::compile(vector<Token>&& tokens, string_view source)
    -> std::expected<Chunk, Report> {
    Compiler parser(std::move(tokens), source);
    if (const auto report = parser.parse_expr(); report.has_value()) {
        return std::unexpected(*report);
    }
    return Chunk(std::move(parser.m_op_codes), std::move(parser.m_literals));
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

static auto kind_to_op_code(TokenKind kind) -> std::optional<OpCode> {
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

/// @brief Parses tokens into an ast of expressions, mutates Parser.
/// @return Pointer to an upcasted expression tree.
auto Compiler::parse_expr() -> std::optional<Report> {
    const Token& token = next();

    if (token.kind == TokenKind::Number) {
        const auto result = parse_number(token.span);
        if (result.has_value()) {
            m_op_codes.push_back(OpCode::Literal);
            m_literals.push_back(*result);
            return {};
        }
        return result.error();
    }

    if (const auto op_code = kind_to_op_code(token.kind); op_code.has_value()) {
        const auto err_lhs = parse_expr();
        if (err_lhs.has_value()) return err_lhs;
        const auto err_rhs = parse_expr();
        if (err_rhs.has_value()) return err_rhs;

        m_op_codes.push_back(*op_code);
        return {};
    }

    return Report(ReportKind::Error,
                  std::format("Expected expression found <{}>", token.name()),
                  {token.span});
}

auto Compiler::parse_number(Span span) -> std::expected<double, Report> {
    string source;
    for (auto chr : span.source(m_source)) {
        if (chr != '_') source.push_back(chr);
    }
    try {
        return std::stod(source);
    } catch (const std::out_of_range& e) {
        return std::unexpected(
            Report(ReportKind::Error, "Number literal too large", {span}));
        // TODO Add note
        // writeln(out, "Note: {} is the maximum",
        //         std::numeric_limits<double>::max());
    } catch (const std::invalid_argument& e) {
        return std::unexpected(
            Report(ReportKind::Error, "Number literal invalid", {span}));
    }
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
