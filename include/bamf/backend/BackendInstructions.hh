#pragma once

#include <bamf/ir/Instruction.hh>

#include <cstdint>

namespace bamf {

class PhysReg : public Value {
    int m_reg;

public:
    explicit PhysReg(int reg) : m_reg(reg) {}

    int reg() const { return m_reg; }
};

class VirtReg : public Value {};

class MoveInst : public Instruction {
    Value *m_dst;
    Value *m_val;

public:
    MoveInst(Value *dst, Value *val) : m_dst(dst), m_val(val) {
        m_dst->add_user(this);
        m_val->add_user(this);
    }

    ~MoveInst() override {
        m_dst->remove_user(this);
        m_val->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_dst(Value *dst) { m_dst = dst; }

    Value *dst() const { return m_dst; }
    Value *val() const { return m_val; }
};

} // namespace bamf
