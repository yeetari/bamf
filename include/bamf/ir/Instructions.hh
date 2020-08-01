#pragma once

#include <bamf/ir/Instruction.hh>

namespace bamf {

struct AllocInst : public Instruction {};

class LoadInst : public Instruction {
    Value *const m_ptr;

public:
    explicit LoadInst(Value *ptr) : m_ptr(ptr) {}

    Value *ptr() const { return m_ptr; }
};

class StoreInst : public Instruction {
    Value *const m_dst;
    Value *const m_src;

public:
    StoreInst(Value *dst, Value *src) : m_dst(dst), m_src(src) {}

    Value *dst() const { return m_dst; }
    Value *src() const { return m_src; }
};

class RetInst : public Instruction {
    Value *const m_ret_val;

public:
    explicit RetInst(Value *ret_val) : m_ret_val(ret_val) {}

    Value *ret_val() const { return m_ret_val; }
};

} // namespace bamf
