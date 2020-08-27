#pragma once

#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>
#include <bamf/x86/MachineInst.hh>

#include <array>
#include <cstdint>

namespace bamf {

class Stream;

} // namespace bamf

namespace bamf::x86 {

enum class OperandInfoType {
    None = 0,
    Constant,
    Imm,
    ModRmGpr,
    ModRmRm,
    OpcodeGpr,
    Rel,
};

struct OperandInfo {
    OperandInfoType type;
    union {
        std::uint8_t constant;
        std::uint8_t imm_width;
    };
};

struct InstructionInfo {
    bool present;
    std::uint8_t base_op;
    Opcode opcode;
    bool has_slash;
    bool mod_rm;
    int default_address_width;
    int default_operand_width;
    std::array<OperandInfo, 4> operands;
    InstructionInfo *slashes;
};

class Decoder {
    Stream *const m_stream;
    std::array<InstructionInfo, 256> m_table{};
    std::array<InstructionInfo, 256> m_table_0f{};

public:
    BAMF_MAKE_NON_COPYABLE(Decoder)
    BAMF_MAKE_NON_MOVABLE(Decoder)

    explicit Decoder(Stream *stream);
    ~Decoder() = default;

    MachineInst next_inst();
    bool has_next() const;
};

} // namespace bamf::x86
