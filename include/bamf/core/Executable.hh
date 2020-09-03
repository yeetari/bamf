#pragma once

#include <cstdint>

namespace bamf {

class InputFile;

struct Executable {
    const std::uint8_t *code;
    std::uint64_t code_size;
};

Executable load_executable(const InputFile &file);

} // namespace bamf
