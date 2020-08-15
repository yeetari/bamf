#pragma once

#include <bamf/ir/Instruction.hh>
#include <bamf/support/Iterator.hh>

#include <unordered_map>

namespace bamf {

struct AllocInst : public Instruction {};

enum class BinaryOp {
    Add,
    Shl,
    Sub,
};

class BinaryInst : public Instruction {
    BinaryOp m_op;
    Value *m_lhs;
    Value *m_rhs;

public:
    BinaryInst(BinaryOp op, Value *lhs, Value *rhs) : m_op(op), m_lhs(lhs), m_rhs(rhs) {
        m_lhs->add_use(&m_lhs);
        m_rhs->add_use(&m_rhs);
        m_lhs->add_user(this);
        m_rhs->add_user(this);
    }

    ~BinaryInst() override {
        if (m_lhs != nullptr) {
            m_lhs->remove_use(&m_lhs);
            m_lhs->remove_user(this);
        }
        if (m_rhs != nullptr) {
            m_rhs->remove_use(&m_rhs);
            m_rhs->remove_user(this);
        }
    }

    BinaryOp op() const { return m_op; }
    Value *lhs() const { return m_lhs; }
    Value *rhs() const { return m_rhs; }
};

class BranchInst : public Instruction {
    BasicBlock *m_dst;

public:
    explicit BranchInst(BasicBlock *dst) : m_dst(dst) {}

    BasicBlock *dst() const { return m_dst; }
};

class CondBranchInst : public Instruction {
    Value *m_cond;
    BasicBlock *m_false_dst;
    BasicBlock *m_true_dst;

public:
    CondBranchInst(Value *cond, BasicBlock *false_dst, BasicBlock *true_dst)
        : m_cond(cond), m_false_dst(false_dst), m_true_dst(true_dst) {
        m_cond->add_use(&m_cond);
        m_cond->add_user(this);
    }

    ~CondBranchInst() override {
        if (m_cond != nullptr) {
            m_cond->remove_use(&m_cond);
            m_cond->remove_user(this);
        }
    }

    Value *cond() const { return m_cond; }
    BasicBlock *false_dst() const { return m_false_dst; }
    BasicBlock *true_dst() const { return m_true_dst; }
};

class LoadInst : public Instruction {
    Value *m_ptr;

public:
    explicit LoadInst(Value *ptr) : m_ptr(ptr) {
        m_ptr->add_use(&m_ptr);
        m_ptr->add_user(this);
    }

    ~LoadInst() override {
        if (m_ptr != nullptr) {
            m_ptr->remove_use(&m_ptr);
            m_ptr->remove_user(this);
        }
    }

    Value *ptr() const { return m_ptr; }
};

class PhiInst : public Instruction {
    std::unordered_map<BasicBlock *, Value *> m_incoming;

public:
    BAMF_MAKE_ITERABLE(m_incoming)

    PhiInst() = default;
    ~PhiInst() override {
        for (auto [block, value] : m_incoming) {
            if (value != nullptr) {
                value->remove_use(&m_incoming[block]);
                value->remove_user(this);
            }
        }
    }

    void add_incoming(BasicBlock *block, Value *value) {
        m_incoming[block] = value;
        value->add_use(&m_incoming[block]);
        value->add_user(this);
    }

    void remove_incoming(BasicBlock *block);
    const std::unordered_map<BasicBlock *, Value *> &incoming() const { return m_incoming; }
};

class StoreInst : public Instruction {
    Value *m_ptr;
    Value *m_val;

public:
    StoreInst(Value *ptr, Value *val) : m_ptr(ptr), m_val(val) {
        m_ptr->add_use(&m_ptr);
        m_val->add_use(&m_val);
        m_ptr->add_user(this);
        m_val->add_user(this);
    }

    ~StoreInst() override {
        if (m_ptr != nullptr) {
            m_ptr->remove_use(&m_ptr);
            m_ptr->remove_user(this);
        }
        if (m_val != nullptr) {
            m_val->remove_use(&m_val);
            m_val->remove_user(this);
        }
    }

    Value *ptr() const { return m_ptr; }
    Value *val() const { return m_val; }
};

class RetInst : public Instruction {
    Value *m_ret_val;

public:
    explicit RetInst(Value *ret_val) : m_ret_val(ret_val) {
        m_ret_val->add_use(&m_ret_val);
        m_ret_val->add_user(this);
    }

    ~RetInst() override {
        if (m_ret_val != nullptr) {
            m_ret_val->remove_use(&m_ret_val);
            m_ret_val->remove_user(this);
        }
    }

    Value *ret_val() const { return m_ret_val; }
};

} // namespace bamf
