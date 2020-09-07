#include <bamf/backend/MoveInserter.hh>

#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/InstVisitor.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <unordered_map>
#include <vector>

namespace bamf {

namespace {

class Visitor : public InstVisitor {
    Function *const m_function;
    BasicBlock *m_block{nullptr};
    std::unordered_map<Value *, VirtReg *> m_reg_map;

    VirtReg *virt_reg(Value *value);

public:
    explicit Visitor(Function *function) : m_function(function) {}

    void set_block(BasicBlock *block) { m_block = block; }

    void visit(AllocInst *) override;
    void visit(BinaryInst *) override;
    void visit(BranchInst *) override;
    void visit(CompareInst *) override;
    void visit(CondBranchInst *) override;
    void visit(LoadInst *) override;
    void visit(PhiInst *) override;
    void visit(StoreInst *) override;
    void visit(RetInst *) override;
};

VirtReg *Visitor::virt_reg(Value *value) {
    if (!m_reg_map.contains(value)) {
        m_reg_map.emplace(value, new VirtReg);
    }
    return m_reg_map.at(value);
}

void Visitor::visit(AllocInst *) {
    assert(false);
}

void Visitor::visit(BinaryInst *binary) {
    auto *phys_lhs = new PhysReg(0);
    auto *phys_rhs = new PhysReg(1);
    m_block->insert<MoveInst>(m_block->position_of(binary), phys_lhs, binary->lhs());
    m_block->insert<MoveInst>(m_block->position_of(binary), phys_rhs, binary->rhs());
    auto *new_binary = m_block->insert<BinaryInst>(m_block->position_of(binary), binary->op(), phys_lhs, phys_rhs);
    m_block->insert<MoveInst>(m_block->position_of(binary), virt_reg(new_binary), phys_lhs);
    binary->replace_all_uses_with(virt_reg(new_binary));
    binary->remove_from_parent();
}

void Visitor::visit(BranchInst *) {}

void Visitor::visit(CompareInst *compare) {
    auto *phys_lhs = new PhysReg(0);
    auto *phys_rhs = new PhysReg(1);
    m_block->insert<MoveInst>(m_block->position_of(compare), phys_lhs, compare->lhs());
    m_block->insert<MoveInst>(m_block->position_of(compare), phys_rhs, compare->rhs());
    auto *new_compare = m_block->insert<CompareInst>(m_block->position_of(compare), compare->pred(), phys_lhs, phys_rhs);
    m_block->insert<MoveInst>(m_block->position_of(compare), virt_reg(new_compare), phys_lhs);
    compare->replace_all_uses_with(virt_reg(new_compare));
    compare->remove_from_parent();
}

void Visitor::visit(CondBranchInst *cond_branch) {
    auto *virt = virt_reg(cond_branch->cond());
    m_block->insert<MoveInst>(m_block->position_of(cond_branch), virt, cond_branch->cond());
    m_block->append<CondBranchInst>(virt, cond_branch->false_dst(), cond_branch->true_dst());
    m_block->remove(cond_branch);
}

void Visitor::visit(LoadInst *) {
    assert(false);
}

void Visitor::visit(PhiInst *phi) {
    auto *virt = virt_reg(phi);
    for (auto [pred, value] : phi->incoming()) {
        auto *pred_terminator = pred->terminator();
        pred->insert<MoveInst>(pred->position_of(pred_terminator), virt, value);
    }
    phi->replace_all_uses_with(virt);
    phi->remove_from_parent();
}

void Visitor::visit(StoreInst *) {
    assert(false);
}

void Visitor::visit(RetInst *ret) {
    auto *virt = new VirtReg;
    m_block->insert<ConstraintInst>(m_block->position_of(ret), virt, Constraint::ReturnValue);
    m_block->insert<MoveInst>(m_block->position_of(ret), virt, ret->ret_val());
}

} // namespace

void MoveInserter::run_on(Function *function) {
    std::vector<Instruction *> work_queue;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            work_queue.push_back(inst.get());
        }
    }

    Visitor visitor(function);
    for (auto *inst : work_queue) {
        visitor.set_block(inst->parent());
        inst->accept(&visitor);
    }
}

} // namespace bamf
