#include <bamf/x86/MachineInst.hh>

#include <bamf/x86/Register.hh>

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace bamf::x86 {

namespace {

void print_operand_size(const MachineInst &inst, std::stringstream &ss) {
    if (inst.operands[0].type == OperandType::None) {
        return;
    }

    bool has_reg = false;
    for (const auto &operand : inst.operands) {
        if (operand.type == OperandType::Label) {
            return;
        }
        has_reg |= operand.type == OperandType::Reg;
    }

    if (has_reg) {
        return;
    }

    switch (inst.operand_width) {
    case 8:
        ss << "byte ";
        break;
    case 16:
        ss << "word ";
        break;
    case 32:
        ss << "dword ";
        break;
    case 64:
        ss << "qword ";
        break;
    }
}

} // namespace

const char *mnemonic(Opcode opcode) {
    switch (opcode) {
    case Opcode::Add:
        return "add";
    case Opcode::And:
        return "and";
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
    case Opcode::Jne:
        return "jne";
    case Opcode::Lea:
        return "lea";
    case Opcode::Mov:
        return "mov";
    case Opcode::Or:
        return "or";
    case Opcode::Pop:
        return "pop";
    case Opcode::Push:
        return "push";
    case Opcode::Ret:
        return "ret";
    case Opcode::Sete:
        return "sete";
    case Opcode::Setl:
        return "setl";
    case Opcode::Setne:
        return "setne";
    case Opcode::Shl:
        return "shl";
    case Opcode::Sub:
        return "sub";
    case Opcode::Xor:
        return "xor";
    default:
        assert(false);
    }
}

std::string dump_inst(const MachineInst &inst, bool pretty) {
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
        print_operand_size(inst, ss);
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
            ss << operand.imm;
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
    return ss.str();
}

bool operator==(const Operand &lhs, const Operand &rhs) {
    if (lhs.type != rhs.type) {
        return false;
    }
    switch (lhs.type) {
    case OperandType::None:
        return true;
    case OperandType::Imm:
    case OperandType::Label:
        return lhs.imm == rhs.imm;
    case OperandType::MemBaseDisp:
        return lhs.base == rhs.base && lhs.disp == rhs.disp;
    case OperandType::MemBaseIndexScale:
        return lhs.base == rhs.base && lhs.index == rhs.index && lhs.scale == rhs.scale;
    case OperandType::Reg:
        return lhs.reg == rhs.reg;
    }
}

bool operator==(const MachineInst &lhs, const MachineInst &rhs) {
    if (lhs.opcode != rhs.opcode) {
        return false;
    }
    if (lhs.address_width != rhs.address_width) {
        return false;
    }
    if (lhs.operand_width != rhs.operand_width) {
        return false;
    }
    for (int i = 0; i < lhs.operands.size(); i++) {
        if (lhs.operands[i] != rhs.operands[i]) {
            return false;
        }
    }
    return true;
}

} // namespace bamf::x86
