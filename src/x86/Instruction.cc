#include <bamf/x86/Instruction.hh>

#include <iostream>
#include <sstream>

namespace bamf::x86 {

void Instruction::dump() {
    std::stringstream ss;
    switch (m_opcode) {
    case Opcode::MovRegImm:
        ss << "mov ";
        ss << reg_to_str(m_reg, 32);
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
