#pragma once

#include <bamf/ir/GlobalVariable.hh>

#include <vector>

namespace bamf {

// TODO: Make this an analysis result
class DecompilationContext {
    std::vector<GlobalVariable *> m_phys_regs;

public:
    void add_phys_reg(GlobalVariable *phys_reg);
    bool is_phys_reg(Value *value) const;

    const std::vector<GlobalVariable *> &phys_regs() const { return m_phys_regs; }
};

} // namespace bamf
