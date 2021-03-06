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
#include <bamf/x86/BackendResult.hh>
#include <bamf/x86/Builder.hh>
#include <bamf/x86/MachineInst.hh>

#include <cassert>
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace bamf::x86 {

namespace {

class InstTranslator : public BackendInstVisitor {
    friend Backend;

private:
    Function *const m_function;
    std::unordered_map<AllocInst *, std::size_t> m_alloc_map;
    std::unordered_map<BasicBlock *, int> m_block_map;
    std::vector<MachineInst> m_insts;

    std::size_t m_frame_size{0};

    Builder emit(Opcode opcode, int operand_width = 64);
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

Builder InstTranslator::emit(Opcode opcode, int operand_width) {
    auto &inst = m_insts.emplace_back();
    inst.opcode = opcode;
    inst.address_width = 64;
    inst.operand_width = operand_width;
    return Builder(&inst);
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

void InstTranslator::visit(CompareInst *compare) {
    auto inst = emit(Opcode::Cmp);
    emit_op(inst, compare->lhs());
    emit_op(inst, compare->rhs());

    auto opcode = [](ComparePred pred) {
      switch (pred) {
      case ComparePred::Eq:
          return Opcode::Sete;
      case ComparePred::Ne:
          return Opcode::Setne;
      case ComparePred::Slt:
          return Opcode::Setl;
      default:
          assert(false);
      }
      assert(false);
    };
    emit(opcode(compare->pred()), 8).reg(Register::Rax);
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

void Backend::run_on(Program *program) {
    assert(program->globals().empty());
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

    // TODO: Emplace instructions directly into BackendResult.
    auto *result = m_manager->make<BackendResult>(function);
    for (auto &inst : translator.m_insts) {
        result->push_back(inst);
    }
}

} // namespace bamf::x86
