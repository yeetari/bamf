#pragma once

#include <bamf/ir/Instruction.hh>

#include <cstdint>

namespace bamf {

class MoveInst : public Instruction {
    std::size_t m_dst;
    Value *m_val;

public:
    MoveInst(std::size_t dst, Value *val) : m_dst(dst), m_val(val) {}

    void accept(InstVisitor *visitor) override;
    void set_dst(std::size_t dst) { m_dst = dst; }

    std::size_t dst() const { return m_dst; }
    Value *val() const { return m_val; }
};

} // namespace bamf
