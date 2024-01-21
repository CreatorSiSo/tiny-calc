#include "parse.hpp"

Parser::Parser(vector<Token>&& tokens, string_view source)
    : m_current(0), m_tokens(tokens), m_source(source) {
    // Insert end of file token
    size_t end = 0;
    if (!m_tokens.empty()) {
        Span last_span = m_tokens.back().span;
        end = last_span.start + last_span.len;
    }
    m_tokens.push_back(Token(TokenKind::EndOfFile, Span(end, 0)));
}

static auto kind_to_operator(TokenKind kind) -> std::optional<BinaryOp> {
    switch (kind) {
        case TokenKind::Plus:
            return BinaryOp::Add;
        case TokenKind::Minus:
            return BinaryOp::Sub;
        case TokenKind::Star:
            return BinaryOp::Mul;
        case TokenKind::Slash:
            return BinaryOp::Div;
        default:
            return {};
    }
}

ParseError::ParseError(ParseError::ExpectedFound error, Span span)
    : kind(error), span(span) {}
ParseError::ParseError(ParseError::Number error, Span span)
    : kind(error), span(span) {}

/// @brief Parses tokens into an ast of expressions, mutates Parser.
/// @return Pointer to an upcasted expression tree.
auto Parser::parse_expr() -> std::expected<unique_ptr<Expr>, ParseError> {
    auto token = next();

    if (token.kind == TokenKind::Number) {
        const auto result = parse_number(token.span);
        if (result.has_value())
            return unique_ptr<Expr>(new Number(*result));
        else
            return std::unexpected(ParseError(result.error(), token.span));
    }

    if (const auto op = kind_to_operator(token.kind); op.has_value()) {
        auto lhs = parse_expr();
        if (!lhs.has_value()) return std::unexpected(lhs.error());
        auto rhs = parse_expr();
        if (!rhs.has_value()) return std::unexpected(rhs.error());

        return unique_ptr<Expr>(
            new BinaryExpr(*op, std::move(*lhs), std::move(*rhs)));
    }

    // TODO What should be done here?
    panic("Todo");
}

auto Parser::parse_number(Span span)
    -> std::expected<double, ParseError::Number> {
    string source;
    for (auto chr : span.source(m_source)) {
        if (chr != '_') source.push_back(chr);
    }
    try {
        return std::stod(source);
    } catch (const std::out_of_range& e) {
        return std::unexpected(ParseError::Number::OutOfRange);
    } catch (const std::invalid_argument& e) {
        return std::unexpected(ParseError::Number::Invalid);
    }
}

auto Parser::expect(TokenKind expected)
    -> std::expected<Token, ParseError::ExpectedFound> {
    const Token& token = next();
    if (token.kind != expected) {
        return std::unexpected(ParseError::ExpectedFound{
            .expected = expected,
            .found = token.kind,
        });
    }
    return token;
}

auto Parser::next() -> const Token& {
    if (m_current < m_tokens.size()) {
        const Token& result = m_tokens.at(m_current);
        m_current += 1;
        return result;
    } else {
        // last token is always end of file token
        return m_tokens.back();
    }
}
