#pragma once

#include <bamf/core/NonCopyable.hh>
#include <bamf/core/NonMovable.hh>
#include <bamf/x86/Instruction.hh>

namespace bamf {

class InputStream;

} // namespace bamf

namespace bamf::x86 {

struct InstructionInfo {
    bool present;
    std::uint8_t base_op;
};

class Decoder {
    InputStream *const m_stream;
    InstructionInfo m_table[256]{};

public:
    BAMF_MAKE_NON_COPYABLE(Decoder)
    BAMF_MAKE_NON_MOVABLE(Decoder)

    Decoder(InputStream *stream);

    Instruction next_inst();
};

} // namespace bamf::x86
