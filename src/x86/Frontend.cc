#include <bamf/x86/Frontend.hh>

#include <bamf/ir/Expressions.hh>
#include <bamf/ir/Statements.hh>

#include <cassert>
#include <stdexcept>

namespace bamf::x86 {

Local *Frontend::local(std::size_t local) {
    return &m_locals.try_emplace(local, std::to_string(local - 16)).first->second;
}

Local *Frontend::reg_local(Register reg) {
    return local(static_cast<std::size_t>(reg));
}

void Frontend::translate_mov(const Operand &dst, const Operand &src) {
    assert(dst.type == OperandType::Reg);
    switch (src.type) {
    case OperandType::Imm:
        m_block->insert<AssignStmt>(reg_local(dst.reg), new ConstExpr(src.imm));
        break;
    case OperandType::Reg:
        m_block->insert<AssignStmt>(reg_local(dst.reg), reg_local(src.reg));
        break;
    default:
        throw std::runtime_error("Unsupported mov src type");
    }
}

void Frontend::translate_pop(const Operand &dst) {
    assert(dst.type == OperandType::Reg);
    const auto *src = m_stack.back();
    m_stack.pop_back();
    m_block->insert<AssignStmt>(reg_local(dst.reg), src);
}

void Frontend::translate_push(const Operand &src) {
    const auto *dst = local(m_stack.size() + 16);
    m_stack.push_back(dst);
    switch (src.type) {
    case OperandType::Imm:
        m_block->insert<AssignStmt>(dst, new ConstExpr(src.imm));
        break;
    case OperandType::Reg:
        m_block->insert<AssignStmt>(dst, reg_local(src.reg));
        break;
    default:
        throw std::runtime_error("Unsupported push src type");
    }
}

void Frontend::translate_ret() {
    m_block->insert<RetStmt>(reg_local(Register::Rax));
}

std::unique_ptr<Function> Frontend::run() {
    auto function = std::make_unique<Function>("main");
    m_block = function->insert_block();
    function->set_entry(m_block);
    for (int i = 0; i < 15; i++) {
        m_locals.emplace(i, reg_to_str(static_cast<Register>(i), 64));
    }

    while (m_decoder->has_next()) {
        auto inst = m_decoder->next_inst();
        dump_inst(inst);
        switch (inst.opcode) {
        case Opcode::Mov:
            translate_mov(inst.operands[0], inst.operands[1]);
            break;
        case Opcode::Pop:
            translate_pop(inst.operands[0]);
            break;
        case Opcode::Push:
            translate_push(inst.operands[0]);
            break;
        case Opcode::Ret:
            translate_ret();
            break;
        default:
            throw std::runtime_error("Unsupported machine instruction");
        }
    }

    return function;
}

} // namespace bamf::x86
