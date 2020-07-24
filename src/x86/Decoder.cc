#include <bamf/x86/Decoder.hh>

#include <bamf/support/Stream.hh>
#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace bamf::x86 {

Decoder::Decoder(Stream *stream) : m_stream(stream) {
    // mov r16, imm16 (B8+ rw iw)
    // mov r32, imm32 (B8+ rd id)
    // mov r64, imm64 (REX.W + B8+ rd io)
    for (std::uint8_t i = 0xB8; i < 0xBF; i++) {
        m_table[i] = {true, 0xB8, Opcode::MovRegImm, DecodeMethod::OpRegImm, false};
    }

    // ret (C3)
    m_table[0xC3] = {true, 0xC3, Opcode::Ret, DecodeMethod::Op, false};

    // call rel16 (E8 cw)
    // call rel32 (E8 cd)
    m_table[0xE8] = {true, 0xE8, Opcode::Call, DecodeMethod::OpImm, false};
}

Instruction Decoder::next_inst() {
    Instruction inst;
    inst.m_offset = m_stream->bytes_read();
    assert(m_stream->has_more());

    auto byte = m_stream->read<std::uint8_t>();
    bool has_prefix = true;
    switch (byte) {
    case 0x66:
        // Operand size override
        inst.m_bit_width = 16;
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

    switch (info.method) {
    case DecodeMethod::OpRegImm:
        inst.m_dst = static_cast<Register>(op - info.base_op);
        // fall-through
    case DecodeMethod::OpImm:
        inst.m_imm = inst.m_bit_width == 16 ? m_stream->read<std::uint16_t>() : m_stream->read<std::uint32_t>();
        // fall-through
    default:
        inst.m_opcode = info.opcode;
        break;
    }

    if (info.mod_rm) {
        auto mod_rm = m_stream->read<std::uint8_t>();
        auto reg = (mod_rm >> 3U) & 0b111U;
        auto rm = mod_rm & 0b111U;

        switch (info.method) {
        case DecodeMethod::OpRegReg:
            inst.m_dst = static_cast<Register>(rm);
            inst.m_src = static_cast<Register>(reg);
            break;
        }
    }

    return inst;
}

} // namespace bamf::x86
