#include <bamf/ir/BasicBlock.hh>

#include <algorithm>

namespace bamf {

BasicBlock::~BasicBlock() {
    for (auto &inst : m_instructions) {
        inst->replace_all_uses_with(nullptr);
    }
}

BasicBlock::const_iterator BasicBlock::position_of(Instruction *inst) const {
    return std::find_if(m_instructions.begin(), m_instructions.end(), [inst](auto &ptr) {
        return ptr.get() == inst;
    });
}

BasicBlock::const_iterator BasicBlock::remove(Instruction *inst) {
    auto it = position_of(inst);
    if (it != m_instructions.end()) {
        inst->replace_all_uses_with(nullptr);
        m_instructions.erase(it);
    }
    return it;
}

Instruction *BasicBlock::terminator() {
    if (m_instructions.empty()) {
        return nullptr;
    }
    return m_instructions.back().get();
}

} // namespace bamf
