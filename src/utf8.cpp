#include "utf8.hpp"

static_assert([] {
    std::array<std::string_view, 4> strings = {"兔", "€", "ß", "t"};
    std::array<uint32_t, 4> scalars = {0x2F80F, 0x20AC, 0xDF, 0x74};
    std::array<int8_t, 4> lengths = {4, 3, 2, 1};

    for (size_t i = 0; i < strings.size(); i += 1) {
        auto [scalar, length] = *UnicodeScalars(strings[i]);
        if (scalar != scalars[i]) return false;
        if (length != lengths[i]) return false;
    }
    return true;
}());

static_assert(utf8_width("兔") == 1);
static_assert(utf8_width("€") == 1);
static_assert(utf8_width("ß") == 1);
static_assert(utf8_width("ÄEIÖÜ") == 5);
static_assert(utf8_width("test") == 4);
