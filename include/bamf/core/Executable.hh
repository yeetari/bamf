#pragma once

#include <cstdint>

namespace bamf {

struct Executable {
    const std::uint8_t *code;
    std::uint64_t code_size;
    std::uint64_t entry_point;
};

} // namespace bamf
