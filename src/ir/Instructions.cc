#include <bamf/ir/Instructions.hh>

#include <bamf/ir/BasicBlock.hh>

namespace bamf {

void BinaryInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

void BranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_dst = m_dst == orig ? repl->as<BasicBlock>() : m_dst;
}

void CompareInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

void CondBranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_cond = m_cond == orig ? repl : m_cond;
    m_false_dst = m_false_dst == orig ? repl->as<BasicBlock>() : m_false_dst;
    m_true_dst = m_true_dst == orig ? repl->as<BasicBlock>() : m_true_dst;
}

void LoadInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
}

void PhiInst::replace_uses_of_with(Value *orig, Value *repl) {
    for (auto [block, value] : m_incoming) {
        if (block == orig) {
            remove_incoming(block);
            add_incoming(repl->as<BasicBlock>(), value);
        }
        if (value == orig) {
            m_incoming[block] = repl;
        }
    }
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

void StoreInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
    m_val = m_val == orig ? repl : m_val;
}

void RetInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ret_val = m_ret_val == orig ? repl : m_ret_val;
}

} // namespace bamf
