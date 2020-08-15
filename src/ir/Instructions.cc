#include <bamf/ir/Instructions.hh>

#include <algorithm>
#include <cassert>

namespace bamf {

void BinaryInst::replace_uses_of_with(Value *a, Value *b) {
    m_lhs = m_lhs == a ? b : m_lhs;
    m_rhs = m_rhs == a ? b : m_rhs;
}

void LoadInst::replace_uses_of_with(Value *a, Value *b) {
    m_ptr = m_ptr == a ? b : m_ptr;
}

void PhiInst::replace_uses_of_with(Value *a, Value *b) {
    // TODO: Implement
    assert(false);
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
