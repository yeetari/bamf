#include <bamf/x86/MachineInst.hh>

#include <cstdint>
#include <iostream>
#include <sstream>

namespace bamf::x86 {

void MachineInst::dump() {
    std::stringstream ss;
    ss << std::hex << m_offset << ": " << std::dec;
    switch (m_opcode) {
    case Opcode::Call:
        ss << "call " << std::hex;
        ss << "0x" << static_cast<std::uint32_t>(m_imm + m_offset + 5);
        break;
    case Opcode::Lea:
        ss << "lea ";
        ss << reg_to_str(m_dst, m_operand_bit_width);
        ss << ", [";
        ss << reg_to_str(m_sib_base, m_address_bit_width);
        ss << " + ";
        ss << reg_to_str(m_sib_index, m_address_bit_width);
        ss << " * ";
        ss << (static_cast<unsigned int>(m_sib_scale) & 0xFFU);
        ss << "]";
        break;
    case Opcode::MovRegImm:
        ss << "mov ";
        ss << reg_to_str(m_dst, m_operand_bit_width);
        ss << ", ";
        ss << m_imm;
        break;
    case Opcode::MovRegReg:
        ss << "mov ";
        ss << reg_to_str(m_dst, m_operand_bit_width);
        ss << ", ";
        ss << reg_to_str(m_src, m_operand_bit_width);
        break;
    case Opcode::PushReg:
        ss << "push ";
        ss << reg_to_str(m_src, m_operand_bit_width);
        break;
    case Opcode::Ret:
        ss << "ret";
        break;
    case Opcode::Xor:
        ss << "xor ";
        ss << reg_to_str(m_dst, m_operand_bit_width);
        ss << ", ";
        ss << reg_to_str(m_src, m_operand_bit_width);
        break;
    }
    std::cout << ss.str() << '\n';
}

} // namespace bamf::x86
