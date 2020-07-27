#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>
#include <bamf/x86/MachineInst.hh>

#include <array>

namespace bamf {

class Stream;

} // namespace bamf

namespace bamf::x86 {

enum class DecodeMethod {
    Op,
    OpImm,
    OpReg,
    OpRegImm,

    // ModRM
    OpRegMem,
    OpRegReg,
};

struct InstructionInfo {
    bool present;
    std::uint8_t base_op;
    Opcode opcode;
    DecodeMethod method;
    bool mod_rm;
    int default_address_bit_width;
    int default_operand_bit_width;
};

class Decoder {
    Stream *const m_stream;
    std::array<InstructionInfo, 256> m_table{};

public:
    BAMF_MAKE_NON_COPYABLE(Decoder)
    BAMF_MAKE_NON_MOVABLE(Decoder)

    Decoder(Stream *stream);

    MachineInst next_inst();

    bool has_next() const;
};

} // namespace bamf::x86
