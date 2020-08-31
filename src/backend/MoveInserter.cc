#include <bamf/backend/MoveInserter.hh>

#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/InstVisitor.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <unordered_map>

namespace bamf {

namespace {

class Visitor : public InstVisitor {
    Function *const m_function;
    BasicBlock *m_block{nullptr};
    std::unordered_map<Value *, std::size_t> m_reg_map;

    std::size_t virt_reg(Value *value);

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

std::size_t Visitor::virt_reg(Value *value) {
    if (!m_reg_map.contains(value)) {
        m_reg_map.emplace(value, m_reg_map.size());
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

void Visitor::visit(PhiInst *) {
    assert(false);
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
    Visitor visitor(function);
    for (auto &block : *function) {
        visitor.set_block(block.get());
        for (auto &inst : *block) {
            inst->accept(&visitor);
        }
    }
}

} // namespace bamf
