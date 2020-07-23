#include <bamf/x86/Decoder.hh>

#include <bamf/io/InputStream.hh>
#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <sstream>
#include <stdexcept>

namespace bamf::x86 {

Decoder::Decoder(InputStream *stream) : m_stream(stream) {
    // mov r16, imm16 (B8+ rw iw)
    // mov r32, imm32 (B8+ rd id)
    // mov r64, imm64 (REX.W + B8+ rd io)
    for (std::uint8_t i = 0xB8; i < 0xBF; i++) {
        m_table[i] = {true, 0xB8};
    }

    // ret (C3)
    m_table[0xC3] = {true, 0xC3};
}

Instruction Decoder::next_inst() {
    Instruction inst;
    assert(m_stream->has_more());

    auto opcode = m_stream->read<std::uint8_t>();
    auto &info = m_table[opcode];
    if (!info.present) {
        std::stringstream ss;
        ss << "Unknown opcode: " << std::hex;
        ss << (static_cast<int>(opcode) & 0xFF);
        throw std::runtime_error(ss.str());
    }

    switch (info.base_op) {
    case 0xB8:
        inst.m_opcode = Opcode::MovRegImm;
        inst.m_reg = static_cast<Register>(opcode - info.base_op);
        inst.m_imm = m_stream->read<std::uint32_t>();
        break;
    case 0xC3:
        inst.m_opcode = Opcode::Ret;
        break;
    }

    return inst;
}

} // namespace bamf::x86
