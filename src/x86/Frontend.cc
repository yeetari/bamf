#include <bamf/x86/Frontend.hh>

#include <bamf/ir/Constant.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/x86/Decoder.hh>

#include <cassert>
#include <stdexcept>

namespace bamf::x86 {

Value *Frontend::phys_dst(Register reg) {
    return m_phys_regs[reg];
}

Value *Frontend::phys_src(Register reg) {
    return m_block->append<LoadInst>(m_phys_regs[reg]);
}

void Frontend::translate_cmp(const Operand &lhs_op, const Operand &rhs_op) {
    assert(lhs_op.type == OperandType::Reg);
    assert(rhs_op.type == OperandType::Imm);
    auto *lhs = phys_src(lhs_op.reg);
    auto *rhs = new Constant(rhs_op.imm);
    auto *cmp = m_block->append<BinaryInst>(BinaryOp::Sub, lhs, rhs);

    // Set overflow flag.
    auto *xor0 = m_block->append<BinaryInst>(BinaryOp::Xor, lhs, rhs);
    auto *xor1 = m_block->append<BinaryInst>(BinaryOp::Xor, lhs, cmp);
    auto *and0 = m_block->append<BinaryInst>(BinaryOp::And, xor0, xor1);
    m_block->append<StoreInst>(m_of, m_block->append<CompareInst>(ComparePred::Slt, and0, new Constant(0)));

    // Set sign flag based on if cmp is less than zero.
    auto *cmp_ltz = m_block->append<CompareInst>(ComparePred::Slt, cmp, new Constant(0));
    m_block->append<StoreInst>(m_sf, cmp_ltz);
}

void Frontend::translate_inc(const Operand &dst) {
    assert(dst.type == OperandType::Reg);
    auto *added = m_block->append<BinaryInst>(BinaryOp::Add, phys_src(dst.reg), new Constant(1));
    m_block->append<StoreInst>(phys_dst(dst.reg), added);
}

void Frontend::translate_jge(const Operand &target, BasicBlock *false_block) {
    auto *of = m_block->append<LoadInst>(m_of);
    auto *sf = m_block->append<LoadInst>(m_sf);
    auto *cond = m_block->append<CompareInst>(ComparePred::Eq, of, sf);
    auto *true_block = m_blocks.at(target.imm);
    m_block->append<CondBranchInst>(cond, false_block, true_block);
}

void Frontend::translate_jmp(const Operand &target) {
    auto *block = m_blocks.at(target.imm);
    m_block->append<BranchInst>(block);
}

void Frontend::translate_mov(const Operand &dst, const Operand &src) {
    Value *store_dst = nullptr;
    switch (dst.type) {
    case OperandType::MemBaseDisp: {
        auto *base = phys_src(dst.base);
        store_dst = m_block->append<BinaryInst>(BinaryOp::Add, base, new Constant(dst.disp));
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
        auto *displaced = m_block->append<BinaryInst>(BinaryOp::Add, base, new Constant(src.disp));
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

    auto *new_sp = m_block->append<BinaryInst>(BinaryOp::Add, sp, new Constant(8));
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
    auto *new_sp = m_block->append<BinaryInst>(BinaryOp::Sub, sp, new Constant(8));
    m_block->append<StoreInst>(new_sp, val);
    m_block->append<StoreInst>(phys_dst(Register::Rsp), new_sp);
}

void Frontend::translate_shl(const Operand &dst, const Operand &src) {
    assert(dst.type == OperandType::Reg);
    assert(src.type == OperandType::Imm);
    auto *shl = m_block->append<BinaryInst>(BinaryOp::Shl, phys_src(dst.reg), new Constant(src.imm));
    m_block->append<StoreInst>(phys_dst(dst.reg), shl);
}

void Frontend::translate_ret() {
    m_block->append<RetInst>(phys_src(Register::Rax));
}

void Frontend::build_jump_targets() {
    Decoder decoder(m_stream);
    while (decoder.has_next()) {
        auto inst = decoder.next_inst();
        auto addr = inst.operands[0].imm;
        switch (inst.opcode) {
        case Opcode::Jge:
        case Opcode::Jmp: {
            if (!m_blocks.contains(addr)) {
                auto *true_block = m_function->insert_block();
                m_blocks.emplace(addr, true_block);
            }

            if (inst.opcode != Opcode::Jmp) {
                auto false_addr = inst.offset + inst.length;
                if (!m_blocks.contains(false_addr)) {
                    auto *false_block = m_function->insert_block();
                    m_blocks.emplace(false_addr, false_block);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}

void Frontend::build_registers() {
    for (int i = 0; i < 16; i++) {
        auto reg = static_cast<Register>(i);
        auto *global = m_program->add_global();
        global->set_name(reg_to_str(reg, 64));
        m_decomp_ctx->add_phys_reg(global);
        m_phys_regs[reg] = global;
    }

    auto make_flag = [&](const char *name) {
        auto *flag = m_program->add_global();
        flag->set_name(name);
        m_decomp_ctx->add_phys_reg(flag);
        return flag;
    };
    m_of = make_flag("of");
    m_sf = make_flag("sf");
}

std::unique_ptr<Program> Frontend::run() {
    auto program = std::make_unique<Program>();
    m_program = program.get();
    m_function = program->create_function("main");
    m_block = m_function->insert_block();
    m_program->set_main(m_function);

    build_jump_targets();
    build_registers();
    m_stream->reset();

    Decoder decoder(m_stream);
    while (decoder.has_next()) {
        auto inst = decoder.next_inst();
        dump_inst(inst);

        if (m_blocks.contains(inst.offset)) {
            m_block = m_blocks.at(inst.offset);
        }

        switch (inst.opcode) {
        case Opcode::Cmp:
            translate_cmp(inst.operands[0], inst.operands[1]);
            break;
        case Opcode::Inc:
            translate_inc(inst.operands[0]);
            break;
        case Opcode::Jge:
            translate_jge(inst.operands[0], m_blocks.at(inst.offset + inst.length));
            break;
        case Opcode::Jmp:
            translate_jmp(inst.operands[0]);
            break;
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
        case Opcode::Shl:
            translate_shl(inst.operands[0], inst.operands[1]);
            break;
        default:
            throw std::runtime_error("Unsupported machine instruction");
        }
    }

    return program;
}

} // namespace bamf::x86
