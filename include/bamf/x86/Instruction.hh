#pragma once

#include <bamf/x86/Register.hh>

#include <cstdint>

namespace bamf::x86 {

class Decoder;

// Opcode generated by final decoder, not actually x86 opcodes
enum class Opcode {
    Call,
    MovRegImm,
    Ret,
};

class Instruction {
    friend Decoder;

private:
    Opcode m_opcode;
    Register m_reg;
    std::size_t m_imm;
    std::uintptr_t m_offset;

    int m_bit_width{32};

public:
    void dump();
};

} // namespace bamf::x86
