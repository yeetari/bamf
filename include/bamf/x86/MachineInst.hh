#pragma once

#include <bamf/x86/Register.hh>

#include <array>
#include <cstdint>
#include <string>

namespace bamf::x86 {

enum class OperandType {
    None = 0,
    Imm,
    Label,
    MemBaseDisp,
    MemBaseIndexScale,
    Reg,
};

struct Operand {
    OperandType type;
    union {
        std::uint64_t imm;
        std::array<std::uint8_t, sizeof(std::uint64_t)> imm_bytes;
        struct {
            Register base;
            union {
                std::int32_t disp;
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
    Add,
    And,
    Call,
    Cmp,
    Inc,
    Jg,
    Jge,
    Jle,
    Jmp,
    Jne,
    Label,
    Lea,
    Mov,
    Or,
    Pop,
    Push,
    Ret,
    Sete,
    Setl,
    Setne,
    Shl,
    Sub,
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

const char *mnemonic(Opcode opcode);
std::string dump_inst(const MachineInst &inst, bool pretty = true);

bool operator==(const Operand &, const Operand &);
bool operator==(const MachineInst &, const MachineInst &);

} // namespace bamf::x86
