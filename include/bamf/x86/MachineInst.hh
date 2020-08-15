#pragma once

#include <bamf/x86/Register.hh>

#include <array>
#include <cstdint>

namespace bamf::x86 {

enum class OperandType {
    None = 0,
    Imm,
    MemBaseDisp,
    MemBaseIndexScale,
    Reg,
};

struct Operand {
    OperandType type;
    union {
        std::size_t imm;
        std::array<std::uint8_t, sizeof(std::size_t)> imm_bytes;
        struct {
            Register base;
            union {
                std::uint32_t disp;
                struct {
                    Register index;
                    std::uint8_t scale;
                };
            };
        };
        Register reg;
    };
};

enum class Opcode {
    Call,
    Cmp,
    Jge,
    Jmp,
    Lea,
    Mov,
    Pop,
    Push,
    Ret,
    Shl,
    Xor,
};

struct MachineInst {
    std::array<std::uint8_t, 16> bytes;
    std::uint8_t length;
    std::uintptr_t offset;

    Opcode opcode;
    std::array<Operand, 4> operands;
    std::uint8_t address_width;
    std::uint8_t operand_width;
};

void dump_inst(const MachineInst &inst);

} // namespace bamf::x86
