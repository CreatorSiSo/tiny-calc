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

struct UnicodeScalar {
    uint32_t scalar;
    uint8_t length;
};

struct UnicodeScalars {
    constexpr UnicodeScalars(string_view string) : m_data(string) {
        m_next = utf8_decode_scalar(m_data);
    }

    constexpr auto begin() const -> UnicodeScalars { return *this; }
    constexpr auto end() const -> UnicodeScalars { return UnicodeScalars(""); }

    constexpr auto operator*() const -> const UnicodeScalar& { return *m_next; }

    constexpr auto operator++() -> UnicodeScalars& {
        if (m_next) {
            m_data = m_data.substr(m_next->length);
            m_next = utf8_decode_scalar(m_data);
        }
        return *this;
    }

    constexpr auto operator!=(const UnicodeScalars& other) const -> bool {
        return m_data != other.m_data;
    }

   private:
    constexpr static auto utf8_decode_scalar(string_view string)
        -> std::optional<UnicodeScalar> {
        if (string.empty()) return {};

        constexpr uint32_t replacement_scalar = 0xFFFD;
        auto start_unit = utf8_decode_unit(string[0]);
        uint8_t length = start_unit.length();
        uint32_t scalar = start_unit.data;

        // index is also the amount of already consumed bytes
        for (uint8_t index = 1; index < length; index += 1) {
            if (string.size() <= index)
                return {{.scalar = replacement_scalar, .length = index}};

            auto follow_unit = utf8_decode_unit(string[index]);
            if (follow_unit.kind != UnitKind::Follow)
                return {{.scalar = replacement_scalar, .length = index}};

            scalar <<= 6;
            scalar |= follow_unit.data;
        }

        return {{.scalar = scalar, .length = length}};
    }

    constexpr static auto utf8_decode_unit(uint8_t byte) -> Unit {
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

   private:
    string_view m_data;
    std::optional<UnicodeScalar> m_next;
};

constexpr auto utf8_width(string_view string) -> size_t {
    size_t amount = 0;

    for (auto _ : UnicodeScalars(string)) {
        // noop to silence unused variable warning
        (void)_;
        amount += 1;
    }

    return amount;
}