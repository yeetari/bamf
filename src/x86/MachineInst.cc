#include <bamf/x86/MachineInst.hh>

#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace bamf::x86 {

const char *mnemonic(Opcode opcode) {
    switch (opcode) {
    case Opcode::Add:
        return "add";
    case Opcode::Call:
        return "call";
    case Opcode::Cmp:
        return "cmp";
    case Opcode::Inc:
        return "inc";
    case Opcode::Jg:
        return "jg";
    case Opcode::Jge:
        return "jge";
    case Opcode::Jle:
        return "jle";
    case Opcode::Jmp:
        return "jmp";
    case Opcode::Lea:
        return "lea";
    case Opcode::Mov:
        return "mov";
    case Opcode::Pop:
        return "pop";
    case Opcode::Push:
        return "push";
    case Opcode::Ret:
        return "ret";
    case Opcode::Shl:
        return "shl";
    case Opcode::Xor:
        return "xor";
    default:
        assert(false);
    }
}

void dump_inst(const MachineInst &inst, bool pretty) {
    std::stringstream ss;
    if (pretty) {
        ss << std::hex << std::setfill(' ') << std::setw(4) << inst.offset << ": ";
        for (int i = 0; i < inst.length; i++) {
            ss << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(inst.bytes[i]) & 0xFFU);
            ss << ' ';
        }

        for (int i = 0; i < 8 - inst.length; i++) {
            ss << "   ";
        }
    }

    if (inst.opcode != Opcode::Label) {
        if (!pretty) {
            ss << "  ";
        }
        ss << mnemonic(inst.opcode) << ' ';
    }
    for (bool first = true; const auto &operand : inst.operands) {
        if (operand.type == OperandType::None) {
            continue;
        }

        if (!first) {
            ss << ", ";
        }
        first = false;

        switch (operand.type) {
        case OperandType::Imm:
            ss << std::hex << operand.imm << std::dec;
            break;
        case OperandType::Label:
            ss << ".L" + std::to_string(operand.imm);
            break;
        case OperandType::MemBaseDisp:
            ss << '[';
            ss << reg_to_str(operand.base, inst.address_width);
            ss << (operand.disp >= 0 ? " + " : " - ");
            ss << std::abs(operand.disp);
            ss << ']';
            break;
        case OperandType::MemBaseIndexScale:
            ss << '[';
            ss << reg_to_str(operand.base, inst.address_width);
            ss << " + ";
            ss << reg_to_str(operand.index, inst.address_width);
            ss << " * ";
            ss << (static_cast<unsigned int>(operand.scale) & 0xFFU);
            ss << ']';
            break;
        case OperandType::Reg:
            ss << reg_to_str(operand.reg, inst.operand_width);
            break;
        default:
            assert(false);
        }
    }
    if (inst.opcode == Opcode::Label) {
        ss << ":";
    }
    std::cout << ss.str() << '\n';
}

} // namespace bamf::x86
