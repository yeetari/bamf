#pragma once

#include <bamf/ir/Instruction.hh>

namespace bamf {

struct AllocInst : public Instruction {};

enum class BinaryOp {
    Add,
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

class StoreInst : public Instruction {
    Value *m_dst;
    Value *m_src;

public:
    StoreInst(Value *dst, Value *src) : m_dst(dst), m_src(src) {
        m_dst->add_use(&m_dst);
        m_src->add_use(&m_src);
        m_dst->add_user(this);
        m_src->add_user(this);
    }

    ~StoreInst() override {
        if (m_dst != nullptr) {
            m_dst->remove_use(&m_dst);
            m_dst->remove_user(this);
        }
        if (m_src != nullptr) {
            m_src->remove_use(&m_src);
            m_src->remove_user(this);
        }
    }

    Value *dst() const { return m_dst; }
    Value *src() const { return m_src; }
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
