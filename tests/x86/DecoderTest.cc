#include <bamf/support/Stream.hh>
#include <bamf/x86/Decoder.hh>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

namespace bamf::x86 {

namespace {

template <typename... Args>
MachineInst decode_single_inst(Args &&... args) {
    std::array<std::uint8_t, sizeof...(Args)> code = {static_cast<std::uint8_t>(args)...};
    Stream stream(code.data(), code.size());
    Decoder decoder(&stream);
    return decoder.next_inst();
}

// TODO: Test all possible registers for each test

TEST(x86DecoderTest, CmpRegImm16) {
    // cmp ax, 7
    auto inst = decode_single_inst(0x66, 0x83, 0xF8, 0x07);
    EXPECT_EQ(inst.opcode, Opcode::Cmp);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 7);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, CmpRegImm32) {
    // cmp eax, 8
    auto inst = decode_single_inst(0x83, 0xF8, 0x08);
    EXPECT_EQ(inst.opcode, Opcode::Cmp);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 8);
    EXPECT_EQ(inst.operand_width, 32);
}

TEST(x86DecoderTest, CmpRegImm64) {
    // cmp rax, 9
    auto inst = decode_single_inst(0x48, 0x83, 0xF8, 0x09);
    EXPECT_EQ(inst.opcode, Opcode::Cmp);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 9);
    EXPECT_EQ(inst.operand_width, 64);
}

TEST(x86DecoderTest, IncReg16) {
    // inc ax
    auto inst = decode_single_inst(0x66, 0xFF, 0xC0);
    EXPECT_EQ(inst.opcode, Opcode::Inc);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, IncReg32) {
    // inc eax
    auto inst = decode_single_inst(0xFF, 0xC0);
    EXPECT_EQ(inst.opcode, Opcode::Inc);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 32);
}

TEST(x86DecoderTest, IncReg64) {
    // inc rax
    auto inst = decode_single_inst(0x48, 0xFF, 0xC0);
    EXPECT_EQ(inst.opcode, Opcode::Inc);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 64);
}

TEST(x86DecoderTest, JgeRel8) {
    auto inst = decode_single_inst(0x7D, 0x05);
    EXPECT_EQ(inst.opcode, Opcode::Jge);
    EXPECT_EQ(inst.operands[0].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[0].imm, 0x07);
    EXPECT_EQ(inst.address_width, 8);
}

TEST(x86DecoderTest, JleRel32) {
    auto inst = decode_single_inst(0x0F, 0x8E, 0x02, 0x00, 0x00, 0x00);
    EXPECT_EQ(inst.opcode, Opcode::Jle);
    EXPECT_EQ(inst.operands[0].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[0].imm, 0x08);
    EXPECT_EQ(inst.address_width, 32);
}

TEST(x86DecoderTest, MovRegImm16) {
    // mov si, 1
    auto inst = decode_single_inst(0x66, 0xBE, 0x01, 0x00);
    EXPECT_EQ(inst.opcode, Opcode::Mov);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rsi);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 1);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, MovRegImm32) {
    // mov edi, 2
    auto inst = decode_single_inst(0xBF, 0x02, 0x00, 0x00, 0x00);
    EXPECT_EQ(inst.opcode, Opcode::Mov);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rdi);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 2);
    EXPECT_EQ(inst.operand_width, 32);
}

TEST(x86DecoderTest, MovRegReg16) {
    // mov ax, dx
    auto inst = decode_single_inst(0x66, 0x89, 0xD0);
    EXPECT_EQ(inst.opcode, Opcode::Mov);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[1].reg, Register::Rdx);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, MovRegReg32) {
    // mov ecx, ebx
    auto inst = decode_single_inst(0x89, 0xD9);
    EXPECT_EQ(inst.opcode, Opcode::Mov);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rcx);
    EXPECT_EQ(inst.operands[1].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[1].reg, Register::Rbx);
    EXPECT_EQ(inst.operand_width, 32);
}

TEST(x86DecoderTest, NearCall32) {
    // call
    auto inst = decode_single_inst(0xE8, 0xFB, 0xFF, 0xFF, 0xFF);
    EXPECT_EQ(inst.opcode, Opcode::Call);
    EXPECT_EQ(inst.operands[0].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[0].imm, 0);
    EXPECT_EQ(inst.address_width, 32);
}

TEST(x86DecoderTest, NearJmpRel8) {
    auto inst = decode_single_inst(0xEB, 0x05);
    EXPECT_EQ(inst.opcode, Opcode::Jmp);
    EXPECT_EQ(inst.operands[0].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[0].imm, 0x07);
    EXPECT_EQ(inst.address_width, 8);
}

TEST(x86DecoderTest, NearJmpRel32) {
    auto inst = decode_single_inst(0xE9, 0x00, 0x00, 0x00, 0x00);
    EXPECT_EQ(inst.opcode, Opcode::Jmp);
    EXPECT_EQ(inst.operands[0].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[0].imm, 0x05);
    EXPECT_EQ(inst.address_width, 32);
}

TEST(x86DecoderTest, NearRet) {
    // ret
    auto inst = decode_single_inst(0xC3);
    EXPECT_EQ(inst.opcode, Opcode::Ret);
}

TEST(x86DecoderTest, PopReg16) {
    // pop bx
    auto inst = decode_single_inst(0x66, 0x5B);
    EXPECT_EQ(inst.opcode, Opcode::Pop);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rbx);
    EXPECT_EQ(inst.operand_width, 16);
}

// TODO: Add test for invalid PopReg32

TEST(x86DecoderTest, PopReg64) {
    // pop rcx
    auto inst = decode_single_inst(0x59);
    EXPECT_EQ(inst.opcode, Opcode::Pop);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rcx);
    EXPECT_EQ(inst.operand_width, 64);
}

TEST(x86DecoderTest, PushReg16) {
    // push ax
    auto inst = decode_single_inst(0x66, 0x50);
    EXPECT_EQ(inst.opcode, Opcode::Push);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 16);
}

// TODO: Add test for invalid PushReg32

TEST(x86DecoderTest, PushReg64) {
    // push rbp
    auto inst = decode_single_inst(0x55);
    EXPECT_EQ(inst.opcode, Opcode::Push);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rbp);
    EXPECT_EQ(inst.operand_width, 64);
}

TEST(x86DecoderTest, ShlRegOne16) {
    // shl ax, 1
    auto inst = decode_single_inst(0x66, 0xD1, 0xE0);
    EXPECT_EQ(inst.opcode, Opcode::Shl);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 1);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, ShlRegOne32) {
    // shl eax, 1
    auto inst = decode_single_inst(0xD1, 0xE0);
    EXPECT_EQ(inst.opcode, Opcode::Shl);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 1);
    EXPECT_EQ(inst.operand_width, 32);
}

TEST(x86DecoderTest, ShlRegOne64) {
    // shl rax, 1
    auto inst = decode_single_inst(0x48, 0xD1, 0xE0);
    EXPECT_EQ(inst.opcode, Opcode::Shl);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Imm);
    EXPECT_EQ(inst.operands[1].imm, 1);
    EXPECT_EQ(inst.operand_width, 64);
}

TEST(x86DecoderTest, XorRegReg16) {
    // xor ax, ax
    auto inst = decode_single_inst(0x66, 0x31, 0xC0);
    EXPECT_EQ(inst.opcode, Opcode::Xor);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[1].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 16);
}

TEST(x86DecoderTest, XorRegReg32) {
    // xor eax, eax
    auto inst = decode_single_inst(0x31, 0xC0);
    EXPECT_EQ(inst.opcode, Opcode::Xor);
    EXPECT_EQ(inst.operands[0].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[0].reg, Register::Rax);
    EXPECT_EQ(inst.operands[1].type, OperandType::Reg);
    EXPECT_EQ(inst.operands[1].reg, Register::Rax);
    EXPECT_EQ(inst.operand_width, 32);
}

} // namespace

} // namespace bamf::x86
