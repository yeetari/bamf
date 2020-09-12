#include <bamf/ir/Instructions.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/InstVisitor.hh>

namespace bamf {

void AllocInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

BinaryInst::BinaryInst(BinaryOp op, Value *lhs, Value *rhs) : m_op(op), m_lhs(lhs), m_rhs(rhs) {
    m_lhs->add_user(this);
    m_rhs->add_user(this);
}

BinaryInst::~BinaryInst() {
    m_lhs->remove_user(this);
    m_rhs->remove_user(this);
}

void BinaryInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void BinaryInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

BranchInst::BranchInst(BasicBlock *dst) : m_dst(dst) {
    m_dst->add_user(this);
}

BranchInst::~BranchInst() {
    m_dst->remove_user(this);
}

void BranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void BranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_dst = m_dst == orig ? repl->as<BasicBlock>() : m_dst;
}

CompareInst::CompareInst(ComparePred pred, Value *lhs, Value *rhs) : m_pred(pred), m_lhs(lhs), m_rhs(rhs) {
    m_lhs->add_user(this);
    m_rhs->add_user(this);
}

CompareInst::~CompareInst() {
    m_lhs->remove_user(this);
    m_rhs->remove_user(this);
}

void CompareInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CompareInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

CondBranchInst::CondBranchInst(Value *cond, BasicBlock *false_dst, BasicBlock *true_dst)
    : m_cond(cond), m_false_dst(false_dst), m_true_dst(true_dst) {
    m_cond->add_user(this);
    m_false_dst->add_user(this);
    m_true_dst->add_user(this);
}

CondBranchInst::~CondBranchInst() {
    m_cond->remove_user(this);
    m_false_dst->remove_user(this);
    m_true_dst->remove_user(this);
}

void CondBranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CondBranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_cond = m_cond == orig ? repl : m_cond;
    m_false_dst = m_false_dst == orig ? repl->as<BasicBlock>() : m_false_dst;
    m_true_dst = m_true_dst == orig ? repl->as<BasicBlock>() : m_true_dst;
}

LoadInst::LoadInst(Value *ptr) : m_ptr(ptr) {
    m_ptr->add_user(this);
}

LoadInst::~LoadInst() {
    m_ptr->remove_user(this);
}

void LoadInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void LoadInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
}

PhiInst::~PhiInst() {
    for (auto [block, value] : m_incoming) {
        block->remove_user(this);
        value->remove_user(this);
    }
}

void PhiInst::add_incoming(BasicBlock *block, Value *value) {
    m_incoming[block] = value;
    block->add_user(this);
    value->add_user(this);
}

void PhiInst::remove_incoming(BasicBlock *block) {
    if (!m_incoming.contains(block)) {
        return;
    }

    auto *value = m_incoming.at(block);
    block->remove_user(this);
    if (value != nullptr) {
        value->remove_user(this);
    }
    m_incoming.erase(block);
}

void PhiInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void PhiInst::replace_uses_of_with(Value *orig, Value *repl) {
    for (auto it = m_incoming.begin(); it != m_incoming.end();) {
        auto *block = it->first;
        auto *value = it->second;
        if (block == orig) {
            auto *repl_block = repl->as<BasicBlock>();
            block->remove_user(this);
            repl_block->add_user(this);
            it = m_incoming.erase(it);
            m_incoming.emplace(repl_block, value);
        } else {
            ++it;
        }
        if (value == orig) {
            m_incoming[block] = repl;
        }
    }
}

StoreInst::StoreInst(Value *ptr, Value *val) : m_ptr(ptr), m_val(val) {
    m_ptr->add_user(this);
    m_val->add_user(this);
}

StoreInst::~StoreInst() {
    m_ptr->remove_user(this);
    m_val->remove_user(this);
}

void StoreInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void StoreInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
    m_val = m_val == orig ? repl : m_val;
}

RetInst::RetInst(Value *ret_val) : m_ret_val(ret_val) {
    m_ret_val->add_user(this);
}

RetInst::~RetInst() {
    m_ret_val->remove_user(this);
}

void RetInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void RetInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ret_val = m_ret_val == orig ? repl : m_ret_val;
}

} // namespace bamf
