#pragma once

#include <bamf/ir/Instruction.hh>

#include <cstdint>

namespace bamf {

class BackendOperand : public Value {};

class PhysReg : public BackendOperand {
    int m_reg;

public:
    explicit PhysReg(int reg) : m_reg(reg) {}

    int reg() const { return m_reg; }
};

class VirtReg : public BackendOperand {};

class MoveInst : public Instruction {
    BackendOperand *m_dst;
    Value *m_val;

public:
    MoveInst(BackendOperand *dst, Value *val) : m_dst(dst), m_val(val) {
        m_dst->add_user(this);
        m_val->add_user(this);
    }

    ~MoveInst() override {
        m_dst->remove_user(this);
        m_val->remove_user(this);
    }

    void accept(InstVisitor *visitor) override;
    void replace_uses_of_with(Value *orig, Value *repl) override;
    void set_dst(BackendOperand *dst) { m_dst = dst; }

    BackendOperand *dst() const { return m_dst; }
    Value *val() const { return m_val; }
};

} // namespace bamf
