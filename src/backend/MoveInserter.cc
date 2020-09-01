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

void Visitor::visit(BinaryInst *) {
    assert(false);
}

void Visitor::visit(BranchInst *) {}

void Visitor::visit(CompareInst *) {
    assert(false);
}

void Visitor::visit(CondBranchInst *) {
    assert(false);
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
    auto *val = ret->ret_val();
    m_block->insert<MoveInst>(m_block->position_of(ret), virt_reg(val), val);
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
