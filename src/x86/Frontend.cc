#include <bamf/x86/Frontend.hh>

#include <bamf/ir/Expressions.hh>
#include <bamf/ir/Statements.hh>

#include <stdexcept>

namespace bamf::x86 {

Local *Frontend::local(std::size_t local) {
    return &m_locals.try_emplace(local, std::to_string(local - 16)).first->second;
}

Local *Frontend::reg_local(Register reg) {
    return local(static_cast<std::size_t>(reg));
}

void Frontend::translate_mov_reg_imm(Register dst, std::size_t imm) {
    m_block->insert<AssignStmt>(reg_local(dst), new ConstExpr<std::size_t>(imm));
}

void Frontend::translate_mov_reg_reg(Register dst, Register src) {
    m_block->insert<AssignStmt>(reg_local(dst), reg_local(src));
}

void Frontend::translate_push_reg(Register reg) {
    const auto *dst = local(m_stack.size() + 16);
    m_stack.push_back(dst);
    m_block->insert<AssignStmt>(dst, reg_local(reg));
}

void Frontend::translate_pop_reg(Register reg) {
    const auto *src = m_stack.back();
    m_stack.pop_back();
    m_block->insert<AssignStmt>(reg_local(reg), src);
}

void Frontend::translate_ret() {
    m_block->insert<RetStmt>(reg_local(Register::Rax));
}

std::unique_ptr<BasicBlock> Frontend::run() {
    auto block = std::make_unique<BasicBlock>();
    m_block = block.get();
    for (int i = 0; i < 15; i++) {
        m_locals.emplace(i, reg_to_str(static_cast<Register>(i), 64));
    }

    while (m_decoder->has_next()) {
        auto inst = m_decoder->next_inst();
        inst.dump();
        switch (inst.opcode()) {
        case Opcode::MovRegImm:
            translate_mov_reg_imm(inst.dst(), inst.imm());
            break;
        case Opcode::MovRegReg:
            translate_mov_reg_reg(inst.dst(), inst.src());
            break;
        case Opcode::PushReg:
            translate_push_reg(inst.src());
            break;
        case Opcode::PopReg:
            translate_pop_reg(inst.dst());
            break;
        case Opcode::Ret:
            translate_ret();
            break;
        default:
            throw std::runtime_error("Unsupported machine instruction");
        }
    }

    return block;
}

} // namespace bamf::x86
