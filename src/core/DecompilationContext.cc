#include <bamf/core/DecompilationContext.hh>

#include <algorithm>

namespace bamf {

void DecompilationContext::add_phys_reg(GlobalVariable *phys_reg) {
    m_phys_regs.push_back(phys_reg);
}

// TODO: This should really take in a `const Value *`
bool DecompilationContext::is_phys_reg(Value *value) const {
    auto *global = value->as<GlobalVariable>();
    if (global == nullptr) {
        return false;
    }
    return std::find(m_phys_regs.begin(), m_phys_regs.end(), global) != m_phys_regs.end();
}

} // namespace bamf
