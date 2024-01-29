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

    constexpr auto length() -> int8_t {
        if (kind == UnitKind::Follow || kind == UnitKind::Invalid) return 1;
        return static_cast<int8_t>(kind);
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

constexpr auto utf8_decode_scalar(string_view string)
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

constexpr auto utf8_amount_scalars(string_view string) -> size_t {
    size_t offset = 0;
    size_t amount = 0;
    while (true) {
        auto result = utf8_decode_scalar(string.substr(offset));
        if (result.second < 0) break;
        offset += result.second;
        amount += 1;
    }
    return amount;
}
