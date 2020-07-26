#include <bamf/support/Stream.hh>
#include <bamf/x86/Decoder.hh>

#include <gtest/gtest.h>

#include <array>
#include <cstdint>

using namespace bamf;
using namespace bamf::x86;

namespace {

template <typename... Args>
MachineInst decode_single_inst(Args &&... args) {
    std::array<char, sizeof...(Args)> code = {static_cast<char>(args)...};
    Stream stream(code.data(), code.size());
    Decoder decoder(&stream);
    return decoder.next_inst();
}

TEST(x86DecoderTest, MovRegImm16) {
    // mov si, 1
    auto inst = decode_single_inst(0x66, 0xBE, 0x01, 0x00);
    EXPECT_EQ(inst.opcode(), Opcode::MovRegImm);
    EXPECT_EQ(inst.dst(), Register::Rsi);
    EXPECT_EQ(inst.imm(), 1);
    EXPECT_EQ(inst.operand_bit_width(), 16);
}

TEST(x86DecoderTest, MovRegImm32) {
    // mov edi, 2
    auto inst = decode_single_inst(0xBF, 0x02, 0x00, 0x00, 0x00);
    EXPECT_EQ(inst.opcode(), Opcode::MovRegImm);
    EXPECT_EQ(inst.dst(), Register::Rdi);
    EXPECT_EQ(inst.imm(), 2);
    EXPECT_EQ(inst.operand_bit_width(), 32);
}

TEST(x86DecoderTest, MovRegReg16) {
    // mov ax, dx
    auto inst = decode_single_inst(0x66, 0x89, 0xD0);
    EXPECT_EQ(inst.opcode(), Opcode::MovRegReg);
    EXPECT_EQ(inst.dst(), Register::Rax);
    EXPECT_EQ(inst.src(), Register::Rdx);
    EXPECT_EQ(inst.operand_bit_width(), 16);
}

TEST(x86DecoderTest, MovRegReg32) {
    // mov ecx, ebx
    auto inst = decode_single_inst(0x89, 0xD9);
    EXPECT_EQ(inst.opcode(), Opcode::MovRegReg);
    EXPECT_EQ(inst.dst(), Register::Rcx);
    EXPECT_EQ(inst.src(), Register::Rbx);
    EXPECT_EQ(inst.operand_bit_width(), 32);
}

TEST(x86DecoderTest, NearCall16) {
    auto inst = decode_single_inst(0x66, 0xE8, 0xFF, 0xFF);
    EXPECT_EQ(inst.opcode(), Opcode::Call);
    EXPECT_EQ(inst.imm(), 0xFFFF);
    EXPECT_EQ(inst.operand_bit_width(), 16);
}

TEST(x86DecoderTest, NearCall32) {
    auto inst = decode_single_inst(0xE8, 0xFF, 0xFF, 0xFF, 0xFF);
    EXPECT_EQ(inst.opcode(), Opcode::Call);
    EXPECT_EQ(inst.imm(), 0xFFFFFFFF);
    EXPECT_EQ(inst.operand_bit_width(), 32);
}

TEST(x86DecoderTest, NearRet) {
    // ret
    auto inst = decode_single_inst(0xC3);
    EXPECT_EQ(inst.opcode(), Opcode::Ret);
}

TEST(x86DecoderTest, PushReg16) {
    // push ax
    auto inst = decode_single_inst(0x66, 0x50);
    EXPECT_EQ(inst.opcode(), Opcode::PushReg);
    EXPECT_EQ(inst.src(), Register::Rax);
    EXPECT_EQ(inst.operand_bit_width(), 16);
}

// TODO: Add test for invalid PushReg32

TEST(x86DecoderTest, PushReg64) {
    // push rbp
    auto inst = decode_single_inst(0x55);
    EXPECT_EQ(inst.opcode(), Opcode::PushReg);
    EXPECT_EQ(inst.src(), Register::Rbp);
    EXPECT_EQ(inst.operand_bit_width(), 64);
}

TEST(x86DecoderTest, XorRegReg16) {
    // xor ax, ax
    auto inst = decode_single_inst(0x66, 0x31, 0xC0);
    EXPECT_EQ(inst.opcode(), Opcode::Xor);
    EXPECT_EQ(inst.dst(), Register::Rax);
    EXPECT_EQ(inst.src(), Register::Rax);
    EXPECT_EQ(inst.operand_bit_width(), 16);
}

TEST(x86DecoderTest, XorRegReg32) {
    // xor eax, eax
    auto inst = decode_single_inst(0x31, 0xC0);
    EXPECT_EQ(inst.opcode(), Opcode::Xor);
    EXPECT_EQ(inst.dst(), Register::Rax);
    EXPECT_EQ(inst.src(), Register::Rax);
    EXPECT_EQ(inst.operand_bit_width(), 32);
}

} // namespace
