#include <bamf/ir/Instructions.hh>

#include <algorithm>
#include <cassert>

namespace bamf {

void BinaryInst::replace_uses_of_with(Value *a, Value *b) {
    m_lhs = m_lhs == a ? b : m_lhs;
    m_rhs = m_rhs == a ? b : m_rhs;
}

void BranchInst::replace_uses_of_with(Value *a, Value *b) {
    m_dst = m_dst == a ? b->as<BasicBlock>() : m_dst;
}

void CondBranchInst::replace_uses_of_with(Value *a, Value *b) {
    m_cond = m_cond == a ? b : m_cond;
    m_false_dst = m_false_dst == a ? b->as<BasicBlock>() : m_false_dst;
    m_true_dst = m_true_dst == a ? b->as<BasicBlock>() : m_true_dst;
}

void LoadInst::replace_uses_of_with(Value *a, Value *b) {
    m_ptr = m_ptr == a ? b : m_ptr;
}

void PhiInst::replace_uses_of_with(Value *a, Value *b) {
    for (auto [block, value] : m_incoming) {
        if (block == a) {
            remove_incoming(block);
            add_incoming(b->as<BasicBlock>(), value);
        }
        if (value == a) {
            m_incoming[block] = b;
        }
    }
}

void PhiInst::remove_incoming(BasicBlock *block) {
    if (!m_incoming.contains(block)) {
        return;
    }

    auto *value = m_incoming.at(block);
    if (value != nullptr) {
        value->remove_user(this);
    }
    m_incoming.erase(block);
}

void StoreInst::replace_uses_of_with(Value *a, Value *b) {
    m_ptr = m_ptr == a ? b : m_ptr;
    m_val = m_val == a ? b : m_val;
}

void RetInst::replace_uses_of_with(Value *a, Value *b) {
    m_ret_val = m_ret_val == a ? b : m_ret_val;
}

} // namespace bamf
