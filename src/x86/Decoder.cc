#include <bamf/x86/Decoder.hh>

#include <bamf/support/Stream.hh>
#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace bamf::x86 {

Decoder::Decoder(Stream *stream) : m_stream(stream) {
    // xor r16, r16 (31 /r)
    // xor r32, r32 (31 /r)
    // xor r64, r64 (REX.W + 31 /r)
    m_table[0x31] = {true, 0x31, Opcode::Xor, DecodeMethod::OpRegReg, true, 0, 32};

    // push r16 (50+rw)
    // push r32 (50+rd)
    // push r64 (50+rd)
    for (std::uint8_t i = 0x50; i < 0x5F; i++) {
        m_table[i] = {true, 0x50, Opcode::PushReg, DecodeMethod::OpReg, false, 0, 64};
    }

    // mov r16, r16 (89 /r)
    // mov r32, r32 (89 /r)
    m_table[0x89] = {true, 0x89, Opcode::MovRegReg, DecodeMethod::OpRegReg, true, 0, 32};

    // mov r16, imm16 (B8+rw iw)
    // mov r32, imm32 (B8+rd id)
    // mov r64, imm64 (REX.W + B8+rd io)
    for (std::uint8_t i = 0xB8; i < 0xC7; i++) {
        m_table[i] = {true, 0xB8, Opcode::MovRegImm, DecodeMethod::OpRegImm, false, 0, 32};
    }

    // ret (C3)
    m_table[0xC3] = {true, 0xC3, Opcode::Ret, DecodeMethod::Op, false, 0, 32};

    // call rel16 (E8 cw)
    // call rel32 (E8 cd)
    m_table[0xE8] = {true, 0xE8, Opcode::Call, DecodeMethod::OpImm, false, 0, 32};
}

Instruction Decoder::next_inst() {
    Instruction inst{};
    inst.m_offset = m_stream->bytes_read();
    assert(m_stream->has_more());

    auto byte = m_stream->read<std::uint8_t>();
    bool has_prefix = true;
    switch (byte) {
    case 0x66:
        // Operand size override
        inst.m_operand_bit_width = 16;
        break;
    case 0x67:
        // Address size override
        inst.m_address_bit_width = 32;
        break;
    default:
        has_prefix = false;
        break;
    }

    auto op = has_prefix ? m_stream->read<std::uint8_t>() : byte;
    auto &info = m_table[op];
    if (!info.present) {
        std::stringstream ss;
        ss << "Unknown opcode: " << std::hex;
        ss << (static_cast<unsigned int>(op) & 0xFFU);
        throw std::runtime_error(ss.str());
    }

    if (inst.m_address_bit_width == 0) {
        // Address size override prefix not present
        inst.m_address_bit_width = info.default_address_bit_width;
    }

    if (inst.m_operand_bit_width == 0) {
        // Operand size override prefix not present
        inst.m_operand_bit_width = info.default_operand_bit_width;
    }

    inst.m_opcode = info.opcode;
    switch (info.method) {
    case DecodeMethod::OpReg:
        inst.m_src = static_cast<Register>(op - info.base_op);
        break;
    case DecodeMethod::OpRegImm:
        inst.m_dst = static_cast<Register>(op - info.base_op);
        // fall-through
    case DecodeMethod::OpImm:
        inst.m_imm = inst.m_operand_bit_width == 16 ? m_stream->read<std::uint16_t>() : m_stream->read<std::uint32_t>();
        break;
    case DecodeMethod::Op:
        break;
    default:
        assert(info.mod_rm);
        auto mod_rm = m_stream->read<std::uint8_t>();
        auto reg = (mod_rm >> 3U) & 0b111U;
        auto rm = mod_rm & 0b111U;

        switch (info.method) {
        case DecodeMethod::OpRegReg:
            inst.m_dst = static_cast<Register>(rm);
            inst.m_src = static_cast<Register>(reg);
            break;
        }
        break;
    }

    return inst;
}

} // namespace bamf::x86
