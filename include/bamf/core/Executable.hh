#pragma once

#include <cstdint>

namespace bamf {

struct Executable {
    const char *code;
    std::uint64_t code_size;
    std::uint64_t entry_point;
};

} // namespace bamf
