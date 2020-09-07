#include <bamf/x86/Backend.hh>

#include <bamf/analyses/ControlFlowAnalyser.hh>
#include <bamf/analyses/ControlFlowAnalysis.hh>
#include <bamf/backend/BackendInstVisitor.hh>
#include <bamf/backend/BackendInstructions.hh>
#include <bamf/backend/MoveInserter.hh>
#include <bamf/backend/RegAllocator.hh>
#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/ir/Program.hh>
#include <bamf/pass/PassUsage.hh>
#include <bamf/x86/MachineInst.hh>

#include <cassert>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

namespace bamf::x86 {

namespace {

class InstTranslator : public BackendInstVisitor {
    friend Backend;

private:
    class Builder {
        MachineInst *const m_inst;
        std::uint8_t m_op_count{};

    public:
        explicit Builder(MachineInst *inst) : m_inst(inst) {}

        Builder &base_disp(Register base, std::int32_t disp);
        Builder &imm(std::uint64_t op);
        Builder &label(int lbl);
        Builder &reg(Register op);
    };

    Function *const m_function;
    std::unordered_map<AllocInst *, std::size_t> m_alloc_map;
    std::unordered_map<BasicBlock *, int> m_block_map;
    std::vector<MachineInst> m_insts;

    std::size_t m_frame_size{0};

    Builder emit(Opcode opcode);
    void emit_op(Builder &inst, Value *op);

public:
    explicit InstTranslator(Function *function) : m_function(function) {}

    void visit(AllocInst *) override;
    void visit(BinaryInst *) override;
    void visit(BranchInst *) override;
    void visit(CompareInst *) override;
    void visit(CondBranchInst *) override;
    void visit(ConstraintInst *) override;
    void visit(LoadInst *) override;
    void visit(MoveInst *) override;
    void visit(PhiInst *) override;
    void visit(StoreInst *) override;
    void visit(RetInst *) override;
};

InstTranslator::Builder &InstTranslator::Builder::base_disp(Register base, std::int32_t disp) {
    m_inst->operands[m_op_count].type = OperandType::MemBaseDisp;
    m_inst->operands[m_op_count].base = base;
    m_inst->operands[m_op_count++].disp = disp;
    return *this;
}

InstTranslator::Builder &InstTranslator::Builder::imm(std::uint64_t op) {
    m_inst->operands[m_op_count].type = OperandType::Imm;
    m_inst->operands[m_op_count++].imm = op;
    return *this;
}

InstTranslator::Builder &InstTranslator::Builder::label(int lbl) {
    m_inst->operands[m_op_count].type = OperandType::Label;
    m_inst->operands[m_op_count++].imm = lbl;
    return *this;
}

InstTranslator::Builder &InstTranslator::Builder::reg(Register op) {
    m_inst->operands[m_op_count].type = OperandType::Reg;
    m_inst->operands[m_op_count++].reg = op;
    return *this;
}

InstTranslator::Builder InstTranslator::emit(Opcode opcode) {
    auto &inst = m_insts.emplace_back();
    inst.opcode = opcode;
    inst.address_width = 64;
    inst.operand_width = 64;
    return InstTranslator::Builder(&inst);
}

void InstTranslator::emit_op(Builder &inst, Value *op) {
    if (auto *alloc = op->as<AllocInst>()) {
        auto offset = m_alloc_map.at(alloc);
        inst.base_disp(Register::Rbp, -offset);
    } else if (auto *constant = op->as<Constant>()) {
        inst.imm(constant->value());
    } else if (auto *phys = op->as<PhysReg>()) {
        inst.reg(static_cast<Register>(phys->reg()));
    } else {
        assert(false);
    }
}

void InstTranslator::visit(AllocInst *alloc) {
    assert(alloc->parent() == m_function->entry());
    assert(!m_alloc_map.contains(alloc));
    m_frame_size += 8;
    m_alloc_map.emplace(alloc, m_frame_size);
}

void InstTranslator::visit(BinaryInst *binary) {
    auto opcode = [](BinaryOp op) {
      switch (op) {
      case BinaryOp::Add:
          return Opcode::Add;
      case BinaryOp::And:
          return Opcode::And;
      case BinaryOp::Or:
          return Opcode::Or;
      case BinaryOp::Sub:
          return Opcode::Sub;
      case BinaryOp::Xor:
          return Opcode::Xor;
      default:
          assert(false);
      }
      assert(false);
    };
    auto inst = emit(opcode(binary->op()));
    emit_op(inst, binary->lhs());
    emit_op(inst, binary->rhs());
}

void InstTranslator::visit(BranchInst *branch) {
    auto dst = m_block_map.at(branch->dst());
    emit(Opcode::Jmp).label(dst);
}

void InstTranslator::visit(CompareInst *) {
    assert(false);
}

void InstTranslator::visit(CondBranchInst *cond_branch) {
    auto cmp = emit(Opcode::Cmp);
    emit_op(cmp, cond_branch->cond());
    cmp.imm(1);
    emit(Opcode::Jne).label(m_block_map.at(cond_branch->false_dst()));
    emit(Opcode::Jmp).label(m_block_map.at(cond_branch->true_dst()));
}

void InstTranslator::visit(ConstraintInst *) {}

void InstTranslator::visit(LoadInst *) {
    assert(false);
}

void InstTranslator::visit(MoveInst *move) {
    auto *val = move->val();
    if (move->dst()->is<AllocInst>() && val->is<AllocInst>()) {
        auto &inst = emit(Opcode::Mov).reg(Register::Rcx);
        emit_op(inst, val);
        val = new PhysReg(static_cast<int>(Register::Rcx));
    }
    auto inst = emit(Opcode::Mov);
    emit_op(inst, move->dst());
    emit_op(inst, val);
}

void InstTranslator::visit(PhiInst *) {
    assert(false);
}

void InstTranslator::visit(StoreInst *) {
    assert(false);
}

void InstTranslator::visit(RetInst *) {
    emit(Opcode::Pop).reg(Register::Rbp);
    emit(Opcode::Ret);
}

} // namespace

void Backend::build_usage(PassUsage *usage) {
    usage->depends_on<ControlFlowAnalyser>();
    usage->depends_on<MoveInserter>();
    usage->depends_on<RegAllocator>();
}

void Backend::run_on(Program *) {
    std::cout << "bits 64\n";
}

void Backend::run_on(Function *function) {
    auto *cfa = m_manager->get<ControlFlowAnalysis>(function);
    auto dfs = cfa->cfg().run<DepthFirstSearch>();
    InstTranslator translator(function);
    for (int i = 0; auto *block : dfs.pre_order()) {
        translator.m_block_map.emplace(block, i++);
    }
    for (auto *block : dfs.pre_order()) {
        translator.emit(Opcode::Label).label(translator.m_block_map.at(block));
        if (block == function->entry()) {
            translator.emit(Opcode::Push).reg(Register::Rbp);
            translator.emit(Opcode::Mov).reg(Register::Rbp).reg(Register::Rsp);
        }
        for (auto &inst : *block) {
            inst->accept(&translator);
        }
    }
    std::cout << "global " << function->name() << '\n';
    std::cout << function->name() << ":\n";
    for (auto &inst : translator.m_insts) {
        dump_inst(inst, false);
    }
}

} // namespace bamf::x86
