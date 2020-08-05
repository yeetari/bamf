#pragma once

#include <bamf/ir/Instruction.hh>

namespace bamf {

struct AllocInst : public Instruction {};

class LoadInst : public Instruction {
    Value *m_ptr;

public:
    explicit LoadInst(Value *ptr) : m_ptr(ptr) { m_ptr->add_use(&m_ptr); }
    ~LoadInst() override {
        if (m_ptr != nullptr) {
            m_ptr->remove_use(&m_ptr);
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
    }

    ~StoreInst() override {
        if (m_dst != nullptr) {
            m_dst->remove_use(&m_dst);
        }
        if (m_src != nullptr) {
            m_src->remove_use(&m_src);
        }
    }

    Value *dst() const { return m_dst; }
    Value *src() const { return m_src; }
};

class RetInst : public Instruction {
    Value *m_ret_val;

public:
    explicit RetInst(Value *ret_val) : m_ret_val(ret_val) { m_ret_val->add_use(&m_ret_val); }
    ~RetInst() override {
        if (m_ret_val != nullptr) {
            m_ret_val->remove_use(&m_ret_val);
        }
    }

    Value *ret_val() const { return m_ret_val; }
};

} // namespace bamf
