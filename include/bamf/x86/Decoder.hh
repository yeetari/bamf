#pragma once

#include <bamf/core/NonCopyable.hh>
#include <bamf/core/NonMovable.hh>
#include <bamf/x86/Instruction.hh>

#include <array>

namespace bamf {

class Stream;

} // namespace bamf

namespace bamf::x86 {

enum class DecodeMethod {
    Op,
    OpImm,
    OpRegImm,
};

struct InstructionInfo {
    bool present;
    std::uint8_t base_op;
    Opcode opcode;
    DecodeMethod method;
};

class Decoder {
    Stream *const m_stream;
    std::array<InstructionInfo, 256> m_table{};

public:
    BAMF_MAKE_NON_COPYABLE(Decoder)
    BAMF_MAKE_NON_MOVABLE(Decoder)

    Decoder(Stream *stream);

    Instruction next_inst();
};

} // namespace bamf::x86
