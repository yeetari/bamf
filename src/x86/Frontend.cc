#include <bamf/x86/Frontend.hh>

#include <bamf/ir/Constant.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <stdexcept>

namespace bamf::x86 {

Value *Frontend::phys_dst(Register reg) {
    return m_phys_regs[reg] = m_block->insert<AllocInst>();
}

Value *Frontend::phys_src(Register reg) {
    return m_block->insert<LoadInst>(m_phys_regs[reg]);
}

void Frontend::translate_mov(const Operand &dst, const Operand &src) {
    assert(dst.type == OperandType::Reg);
    switch (src.type) {
    case OperandType::Imm:
        m_block->insert<StoreInst>(phys_dst(dst.reg), new Constant(src.imm));
        break;
    case OperandType::Reg:
        m_block->insert<StoreInst>(phys_dst(dst.reg), phys_src(src.reg));
        break;
    default:
        throw std::runtime_error("Unsupported mov src type");
    }
}

void Frontend::translate_pop(const Operand &dst) {
    assert(dst.type == OperandType::Reg);
    auto *src = m_block->insert<LoadInst>(m_stack.back());
    m_stack.pop_back();
    m_block->insert<StoreInst>(phys_dst(dst.reg), src);
}

void Frontend::translate_push(const Operand &src) {
    auto *stack_var = m_block->insert<AllocInst>();
    stack_var->set_name("svar" + std::to_string(m_stack.size()));
    m_stack.push_back(stack_var);

    switch (src.type) {
    case OperandType::Imm:
        m_block->insert<StoreInst>(stack_var, new Constant(src.imm));
        break;
    case OperandType::Reg:
        m_block->insert<StoreInst>(stack_var, phys_src(src.reg));
        break;
    default:
        throw std::runtime_error("Unsupported push src type");
    }
}

void Frontend::translate_ret() {
    m_block->insert<RetInst>(phys_src(Register::Rax));
}

std::unique_ptr<Function> Frontend::run() {
    auto function = std::make_unique<Function>("main");
    m_block = function->insert_block();
    function->set_entry(m_block);

//    for (int reg = 0; reg < 16; reg++) {
//        auto *alloc = m_block->insert<AllocInst>();
//        alloc->set_name(reg_to_str(static_cast<Register>(reg), 64));
//        m_phys_regs.emplace(static_cast<Register>(reg), alloc);
//    }

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
