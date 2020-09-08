#pragma once

#include <bamf/x86/MachineInst.hh>
#include <bamf/x86/Register.hh>

#include <cstdint>

namespace bamf::x86 {

class Builder {
    MachineInst *const m_inst;
    std::uint8_t m_op_count{};

public:
    explicit Builder(MachineInst *inst) : m_inst(inst) {}

    Builder &opcode(Opcode opcode);
    Builder &width(int address_width, int operand_width);

    Builder &base_disp(Register base, std::int32_t disp);
    Builder &imm(std::uint64_t op);
    Builder &label(int lbl);
    Builder &reg(Register op);
};

} // namespace bamf::x86
