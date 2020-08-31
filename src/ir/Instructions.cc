#include <bamf/ir/Instructions.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/InstVisitor.hh>

namespace bamf {

void AllocInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void BinaryInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void BinaryInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

void BranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void BranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_dst = m_dst == orig ? repl->as<BasicBlock>() : m_dst;
}

void CompareInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CompareInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_lhs = m_lhs == orig ? repl : m_lhs;
    m_rhs = m_rhs == orig ? repl : m_rhs;
}

void CondBranchInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void CondBranchInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_cond = m_cond == orig ? repl : m_cond;
    m_false_dst = m_false_dst == orig ? repl->as<BasicBlock>() : m_false_dst;
    m_true_dst = m_true_dst == orig ? repl->as<BasicBlock>() : m_true_dst;
}

void LoadInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void LoadInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
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

void StoreInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void StoreInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ptr = m_ptr == orig ? repl : m_ptr;
    m_val = m_val == orig ? repl : m_val;
}

void RetInst::accept(InstVisitor *visitor) {
    visitor->visit(this);
}

void RetInst::replace_uses_of_with(Value *orig, Value *repl) {
    m_ret_val = m_ret_val == orig ? repl : m_ret_val;
}

} // namespace bamf
