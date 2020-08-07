#include <bamf/ir/BasicBlock.hh>

#include <algorithm>

namespace bamf {

BasicBlock::~BasicBlock() {
    for (auto &inst : m_instructions) {
        inst->replace_all_uses_with(nullptr);
    }
}

BasicBlock::const_iterator BasicBlock::remove(Instruction *inst) {
    auto it = std::find_if(m_instructions.begin(), m_instructions.end(), [inst](auto &ptr) {
        return ptr.get() == inst;
    });

    if (it != m_instructions.end()) {
        inst->replace_all_uses_with(nullptr);
        m_instructions.erase(it);
    }
    return it;
}

} // namespace bamf
