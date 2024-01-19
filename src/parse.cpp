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

/// @brief Parses tokens into an ast of expressions, mutates Parser.
/// @return Pointer to an upcasted expression tree.
auto Parser::parse_expr() -> std::expected<unique_ptr<Expr>, ParseExprError> {
    auto token = next();

    if (token.kind == TokenKind::Number) {
        if (const auto inner = parse_number(token.span); inner.has_value()) {
            return unique_ptr<Expr>(new Number(*inner));
        }
        // TODO
        return nullptr;
    }

    if (const auto op = kind_to_operator(token.kind); op.has_value()) {
        auto lhs = parse_expr();
        if (!lhs.has_value()) {
            return std::unexpected(lhs.error());
        }
        auto rhs = parse_expr();
        if (!rhs.has_value()) {
            return std::unexpected(rhs.error());
        }
        return unique_ptr<Expr>(
            new BinaryExpr(*op, std::move(*lhs), std::move(*rhs)));
    }

    // TODO What should be done here?
    abort();
}

auto Parser::parse_number(Span span)
    -> std::expected<double, ParseNumberError> {
    string source;
    for (auto chr : span.source(m_source)) {
        if (chr != '_') source.push_back(chr);
    }
    return std::stod(source);
}

auto Parser::expect(TokenKind expected)
    -> std::expected<Token, ExpectedFoundError> {
    const Token& token = next();
    if (token.kind != expected) {
        return std::unexpected(ExpectedFoundError{
            .expected = expected,
            .found = token,
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
