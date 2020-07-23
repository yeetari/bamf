#include <bamf/x86/Instruction.hh>

#include <cstdint>
#include <iostream>
#include <sstream>

namespace bamf::x86 {

void Instruction::dump() {
    std::stringstream ss;
    ss << std::hex << m_offset << ": " << std::dec;
    switch (m_opcode) {
    case Opcode::Call:
        ss << "call " << std::hex;
        ss << "0x" << static_cast<std::uint32_t>(m_imm + m_offset + 5);
        break;
    case Opcode::MovRegImm:
        ss << "mov ";
        ss << reg_to_str(m_reg, m_bit_width);
        ss << ", ";
        ss << m_imm;
        break;
    case Opcode::Ret:
        ss << "ret";
        break;
    }
    std::cout << ss.str() << '\n';
}

} // namespace bamf::x86
