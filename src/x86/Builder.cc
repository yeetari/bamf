#include <bamf/x86/Builder.hh>

namespace bamf::x86 {

Builder &Builder::opcode(Opcode opcode) {
    m_inst->opcode = opcode;
    return *this;
}

Builder &Builder::width(int address_width, int operand_width) {
    m_inst->address_width = address_width;
    m_inst->operand_width = operand_width;
    return *this;
}

Builder &Builder::base_disp(Register base, std::int32_t disp) {
    m_inst->operands[m_op_count].type = OperandType::MemBaseDisp;
    m_inst->operands[m_op_count].base = base;
    m_inst->operands[m_op_count++].disp = disp;
    return *this;
}

Builder &Builder::imm(std::uint64_t op) {
    m_inst->operands[m_op_count].type = OperandType::Imm;
    m_inst->operands[m_op_count++].imm = op;
    return *this;
}

Builder &Builder::label(int lbl) {
    m_inst->operands[m_op_count].type = OperandType::Label;
    m_inst->operands[m_op_count++].imm = lbl;
    return *this;
}

Builder &Builder::reg(Register op) {
    m_inst->operands[m_op_count].type = OperandType::Reg;
    m_inst->operands[m_op_count++].reg = op;
    return *this;
}

} // namespace bamf::x86
