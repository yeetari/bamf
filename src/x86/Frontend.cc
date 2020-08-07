#include <bamf/x86/Frontend.hh>

#include <bamf/ir/Constant.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <stdexcept>

namespace bamf::x86 {

Value *Frontend::phys_dst(Register reg) {
    return m_phys_regs[reg];
}

Value *Frontend::phys_src(Register reg) {
    return m_block->append<LoadInst>(m_phys_regs[reg]);
}

void Frontend::translate_mov(const Operand &dst, const Operand &src) {
    Value *store_dst = nullptr;
    switch (dst.type) {
    case OperandType::MemBaseDisp: {
        auto *base = phys_src(dst.base);
        store_dst = m_block->append<BinaryInst>(BinaryOp::Add, base, new Constant<std::size_t>(dst.disp));
        break;
    }
    case OperandType::Reg:
        store_dst = phys_dst(dst.reg);
        break;
    default:
        throw std::runtime_error("Unsupported mov dst type");
    }

    switch (src.type) {
    case OperandType::Imm:
        m_block->append<StoreInst>(store_dst, new Constant(src.imm));
        break;
    case OperandType::MemBaseDisp: {
        auto *base = phys_src(src.base);
        auto *displaced = m_block->append<BinaryInst>(BinaryOp::Add, base, new Constant<std::size_t>(src.disp));
        m_block->append<StoreInst>(store_dst, m_block->append<LoadInst>(displaced));
        break;
    }
    case OperandType::Reg:
        m_block->append<StoreInst>(store_dst, phys_src(src.reg));
        break;
    default:
        throw std::runtime_error("Unsupported mov src type");
    }
}

void Frontend::translate_pop(const Operand &dst) {
    assert(dst.type == OperandType::Reg);
    auto *sp = phys_src(Register::Rsp);
    auto *stack_top = m_block->append<LoadInst>(sp);
    m_block->append<StoreInst>(phys_dst(dst.reg), stack_top);

    auto *new_sp = m_block->append<BinaryInst>(BinaryOp::Add, sp, new Constant<std::size_t>(8));
    m_block->append<StoreInst>(phys_dst(Register::Rsp), new_sp);
}

void Frontend::translate_push(const Operand &src) {
    Value *val = nullptr;
    switch (src.type) {
    case OperandType::Imm:
        val = new Constant(src.imm);
        break;
    case OperandType::Reg:
        val = phys_src(src.reg);
        break;
    default:
        throw std::runtime_error("Unsupported push src type");
    }

    auto *sp = phys_src(Register::Rsp);
    auto *new_sp = m_block->append<BinaryInst>(BinaryOp::Sub, sp, new Constant<std::size_t>(8));
    m_block->append<StoreInst>(new_sp, val);
    m_block->append<StoreInst>(phys_dst(Register::Rsp), new_sp);
}

void Frontend::translate_ret() {
    m_block->append<RetInst>(phys_src(Register::Rax));
}

std::unique_ptr<Program> Frontend::run() {
    auto program = std::make_unique<Program>();
    m_program = program.get();
    m_function = program->create_function("main");
    m_block = m_function->insert_block();
    m_program->set_main(m_function);
    m_function->set_entry(m_block);

    for (int i = 0; i < 16; i++) {
        auto reg = static_cast<Register>(i);
        auto *global = m_program->add_global();
        global->set_name(reg_to_str(reg, 64));
        m_phys_regs[reg] = global;
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

    return program;
}

} // namespace bamf::x86
