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
    None,
    Constant,
    Imm,
    ModRmGpr,
    ModRmRm,
    OpcodeGpr,
    Rel,
};

struct OperandInfo {
    OperandInfoType type{OperandInfoType::None};
    union {
        std::uint8_t constant;
        std::uint8_t imm_width;
    };
};

struct InstructionInfo {
    bool present{false};
    std::uint8_t base_op{0};
    Opcode opcode{};
    bool has_slash{false};
    bool mod_rm{false};
    int default_address_width{0};
    int default_operand_width{0};
    std::array<OperandInfo, 4> operands{};
    InstructionInfo *slashes{nullptr};
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
