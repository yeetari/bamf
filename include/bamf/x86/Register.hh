#pragma once

#include <cstdint>

namespace bamf::x86 {

enum class Register : std::uint8_t {
    Al = 0,
    Ax = 0,
    Eax = 0,
    Rax = 0,

    Cl = 1,
    Cx = 1,
    Ecx = 1,
    Rcx = 1,
};

const char *reg_to_str(Register reg, int bit_width);

} // namespace bamf::x86
