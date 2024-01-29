#include "tokenize.hpp"

#include <cctype>

auto Token::name() const -> string_view {
    switch (kind) {
        case TokenKind::Ident:
            return "Ident";
        case TokenKind::Plus:
            return "Plus";
        case TokenKind::Minus:
            return "Minus";
        case TokenKind::Star:
            return "Star";
        case TokenKind::Slash:
            return "Slash";
        case TokenKind::Number:
            return "Number";
        case TokenKind::Error:
            return "Error";
        case TokenKind::EndOfInput:
            return "EndOfInput";
        default:
            panic("Case <{}> not covered", (uint8_t)kind);
    }
}

auto Token::src(string_view src) const -> string_view {
    return span.source(src);
}

auto Token::debug(string_view src) const -> string {
    return std::format("{}[{}] {}", name(), this->src(src), span.debug());
}

static auto validate_whitespace(string_view str) -> size_t {
    size_t len = 0;
    for (char chr : str) {
        if (!std::isspace(chr)) break;
        len += 1;
    }
    return len;
}

static auto validate_number(string_view str) -> size_t {
    if (!std::isdigit(str.at(0))) return 0;

    size_t len = 1;
    auto integers = str.substr(len);
    bool has_decimal_part = false;

    auto valid_char = [](char c) { return std::isdigit(c) || c == '_'; };

    for (char c : integers) {
        if (c == '.') {
            len += 1;
            has_decimal_part = true;
            break;
        }
        if (!valid_char(c)) break;
        len += 1;
    }

    if (has_decimal_part) {
        for (char c : str.substr(len)) {
            if (!valid_char(c)) break;
            len += 1;
        }
    }

    return len;
}

enum class UnitKind {
    Invalid = -2,
    Follow = -1,
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
};

struct Unit {
    UnitKind kind;
    uint8_t data;

    constexpr Unit(UnitKind kind, uint8_t data) : kind(kind), data(data) {}

    constexpr auto length() -> int8_t {
        if (kind == UnitKind::Follow || kind == UnitKind::Invalid) return 1;
        return static_cast<int8_t>(kind);
    }
};

static constexpr auto utf8_decode_unit(uint8_t byte) -> Unit {
    // First bit is zero
    if ((byte >> 7) == 0b0) {
        return Unit(UnitKind::One, byte);
    }

    // First bit is one and second bit is zero
    if ((byte >> 6) == 0b10) {
        return Unit(UnitKind::Follow, byte & 0b00111111);
    }

    // First two bits are one and third bit is zero
    if ((byte >> 5) == 0b110) {
        return Unit(UnitKind::Two, byte & 0b00011111);
    }

    // First three bits are one and fourth bit is zero
    if ((byte >> 4) == 0b1110) {
        return Unit(UnitKind::Three, byte & 0b00001111);
    }

    // First four bits are one and the fifth is zero
    if ((byte >> 3) == 0b11110) {
        return Unit(UnitKind::Four, byte & 0b00000111);
    }

    return Unit(UnitKind::Invalid, 0);
}

static constexpr auto utf8_decode_scalar(string_view string)
    -> std::pair<uint32_t, int8_t> {
    constexpr uint32_t replacement_scalar = 0xFFFD;

    if (string.empty()) return std::make_pair(replacement_scalar, -1);

    auto start_unit = utf8_decode_unit(string[0]);
    int8_t length = start_unit.length();
    uint32_t scalar = start_unit.data;

    // index is also the amount of already consumed bytes
    for (uint8_t index = 1; index < length; index += 1) {
        if (string.size() <= index)
            return std::make_pair(replacement_scalar, index);

        auto follow_unit = utf8_decode_unit(string[index]);
        if (follow_unit.kind != UnitKind::Follow)
            return std::make_pair(replacement_scalar, index);

        scalar <<= 6;
        scalar |= follow_unit.data;
    }

    return std::make_pair(scalar, length);
}

static_assert([] {
    std::array<string_view, 3> strings = {"€", "ß", "t"};
    std::array<uint32_t, 3> scalars = {0x20AC, 0xDF, 0x74};
    std::array<int8_t, 3> lengths = {3, 2, 1};

    for (size_t i = 0; i < strings.size(); i += 1) {
        auto decoded = utf8_decode_scalar(strings[i]);
        if (decoded.first != scalars[i]) return false;
        if (decoded.second != lengths[i]) return false;
    }
    return true;
}());

static constexpr auto validate_identifier(string_view src) -> size_t {
    size_t ident_length = 0;

    while (true) {
        auto next = utf8_decode_scalar(src.substr(ident_length));
        uint32_t scalar = next.first;
        int8_t length = next.second;

        if (length < 0 || std::iswspace(scalar) || std::iswpunct(scalar) ||
            std::iswdigit(scalar))
            break;
        ident_length += length;
    }

    return ident_length;
}

auto tokenize(string_view str) -> vector<Token> {
    vector<Token> tokens;
    // First character index of current token
    size_t start = 0;

    auto push = [&tokens](TokenKind kind, Span span) {
        tokens.push_back(Token(kind, span));
    };

    while (start < str.length()) {
        auto rest = str.substr(start);
        char chr = str.at(start);

        size_t whitespace_len = validate_whitespace(rest);
        if (whitespace_len > 0) {
            start += whitespace_len;
            continue;
        }

        size_t number_len = validate_number(rest);
        if (number_len > 0) {
            push(TokenKind::Number, Span(start, number_len));
            start += number_len;
            continue;
        }

        size_t identifier_len = validate_identifier(rest);
        if (identifier_len > 0) {
            push(TokenKind::Ident, Span(start, identifier_len));
            start += identifier_len;
            continue;
        }

        // Operators
        switch (chr) {
            case '+':
                push(TokenKind::Plus, Span(start, 1));
                break;
            case '-':
                push(TokenKind::Minus, Span(start, 1));
                break;
            case '*':
                push(TokenKind::Star, Span(start, 1));
                break;
            case '/':
                push(TokenKind::Slash, Span(start, 1));
                break;
            default:
                push(TokenKind::Error, Span(start, 1));
                break;
        }
        start += 1;
    }

    return tokens;
}
