#pragma once

#include "common.hpp"

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

    constexpr auto length() -> uint8_t {
        if (kind == UnitKind::Follow || kind == UnitKind::Invalid) return 1;
        return static_cast<uint8_t>(kind);
    }
};

constexpr auto utf8_decode_unit(uint8_t byte) -> Unit {
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

struct Scalar {
    uint32_t value;
    uint8_t length;
};

constexpr auto utf8_decode_scalar(string_view string) -> Scalar {
    constexpr uint32_t replacement_scalar = 0xFFFD;

    // if (string.empty()) return {.value = replacement_scalar, .length = -1};

    auto start_unit = utf8_decode_unit(string[0]);
    uint8_t length = start_unit.length();
    uint32_t scalar = start_unit.data;

    // index is also the amount of already consumed bytes
    for (uint8_t index = 1; index < length; index += 1) {
        if (string.size() <= index)
            return {.value = replacement_scalar, .length = index};

        auto follow_unit = utf8_decode_unit(string[index]);
        if (follow_unit.kind != UnitKind::Follow)
            return {.value = replacement_scalar, .length = index};

        scalar <<= 6;
        scalar |= follow_unit.data;
    }

    return {.value = scalar, .length = length};
}

struct Chars {
    constexpr Chars(string_view string) : m_data(string) {}

    constexpr auto next() -> std::optional<uint32_t> {
        if (m_data.empty()) return {};
        auto [scalar, length] = utf8_decode_scalar(m_data);
        m_data = m_data.substr(length);
        return scalar;
    }

   private:
    string_view m_data;
};

struct StringView {
    constexpr StringView(string_view data) : m_data(data) {}

    constexpr auto chars() -> Chars { return Chars(m_data); }

    constexpr auto width() -> size_t {
        Chars chars_iter(m_data);
        size_t amount = 0;

        while (true) {
            auto next = chars_iter.next();
            if (!next.has_value()) break;
            amount += 1;
        }

        return amount;
    }

    constexpr auto begin() const -> const char* { return m_data.begin(); }
    constexpr auto end() const -> const char* { return m_data.end(); }

    constexpr auto operator==(string_view other) -> bool {
        return m_data == other;
    }

   private:
    string_view m_data;
};

static_assert(sizeof(Chars) == sizeof(StringView));
static_assert(sizeof(StringView) == sizeof(string_view));
static_assert(std::is_trivially_copyable<StringView>{});
