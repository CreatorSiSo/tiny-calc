#include "utf8.hpp"

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

static_assert(utf8_amount_scalars("€") == 1);
static_assert(utf8_amount_scalars("ß") == 1);
static_assert(utf8_amount_scalars("ÄEIÖÜ") == 5);
static_assert(utf8_amount_scalars("test") == 4);
