#pragma once

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>

namespace utf8 {

enum class UnitKind : int8_t {
    Invalid = -2,
    Follow = -1,
    One = 1,
    Two = 2,
    Three = 3,
    Four = 4,
};

/**
 * @brief Decoded data of a single UTF-8 unit (byte).
 */
struct Unit {
    UnitKind kind;
    uint8_t data;

    constexpr Unit(UnitKind kind, uint8_t data) : kind(kind), data(data) {}

    /**
     * @brief Amount of bytes (1 to 4) needed to encode the entire scalar.
     *
     * Length is one if the unit is not a valid start byte.
     */
    constexpr auto length() -> uint8_t {
        if (kind == UnitKind::Follow || kind == UnitKind::Invalid) {
            return 1;
        }
        return static_cast<uint8_t>(kind);
    }
};

/**
 * @brief A single unicode scalar value and the amount of utf8 bytes it was
 *        decoded from utf8.
 *
 * The length is always the amount of bytes that the scalar was decoded from.
 * So a scalar of value U+FFFD (REPLACEMENT CHARACTER) might have any length
 * from 1 to 4, as it is used when a decoding error is encountered.
 */
struct Scalar {
    uint32_t scalar;
    uint8_t length;
};

/**
 * @brief Iterator over unicode scalars of a utf8 encoded `string_view`.
 */
struct Scalars {
    constexpr Scalars(std::string_view string) : m_data(string) {
        m_next = decode_scalar(m_data);
    }

    constexpr auto begin() const -> Scalars { return *this; }
    constexpr auto end() const -> Scalars { return Scalars(""); }

    constexpr auto operator*() const -> const Scalar& { return *m_next; }

    constexpr auto operator++() -> Scalars& {
        if (m_next) {
            m_data = m_data.substr(m_next->length);
            m_next = decode_scalar(m_data);
        }
        return *this;
    }

    constexpr auto operator!=(const Scalars& other) const -> bool {
        return m_data != other.m_data;
    }

   private:
    /**
     * @brief Decodes a unicode scalar from the first 1 to 4 bytes.
     * @param string Input, that is going to be decoded.
     * @return The decoded unicode scalar or nothing if `string` is empty.
     */
    constexpr static auto decode_scalar(std::string_view string)
        -> std::optional<Scalar> {
        if (string.empty()) {
            return {};
        }

        constexpr uint32_t replacement_scalar = 0xFFFD;
        auto start_unit = decode_unit(string[0]);
        uint8_t length = start_unit.length();
        uint32_t scalar = start_unit.data;

        // index is also the amount of already consumed bytes
        for (uint8_t index = 1; index < length; index += 1) {
            if (string.size() <= index) {
                return {{.scalar = replacement_scalar, .length = index}};
            }

            auto follow_unit = decode_unit(string[index]);
            if (follow_unit.kind != UnitKind::Follow) {
                return {{.scalar = replacement_scalar, .length = index}};
            }

            scalar <<= 6;
            scalar |= follow_unit.data;
        }

        return {{.scalar = scalar, .length = length}};
    }

    /**
     * @brief Reads the start bits and value stored in a single utf8 unit (byte)
     * @param byte Byte to decode.
     * @return Decoded data (value and length).
     */
    constexpr static auto decode_unit(uint8_t byte) -> Unit {
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
    std::string_view m_data;
    std::optional<Scalar> m_next;
};

/**
 * @brief Approximate width of the input when decoded as utf8 and displayed.
 *
 * Does not properly handle graphemes that use more than one unicode scalar,
 * or have a display width larger than one. (Emojis like this: 🫵🏿, ...)
 *
 * @param string Utf8 encoded input.
 * @return Amount of unicode scalars.
 */
constexpr auto width(std::string_view string) -> size_t {
    size_t amount = 0;

    for (Scalar scalar : Scalars(string)) {
        // noop to silence unused variable warning
        static_cast<void>(scalar);
        amount += 1;
    }

    return amount;
}

}  // namespace utf8
