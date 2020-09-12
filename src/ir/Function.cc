#include <bamf/ir/Function.hh>

#include <algorithm>

namespace bamf {

BasicBlock *Function::append_block() {
    auto *block = m_blocks.emplace_back(new BasicBlock).get();
    block->set_parent(this);
    return block;
}

Function::const_iterator Function::position_of(BasicBlock *block) const {
    return std::find_if(m_blocks.begin(), m_blocks.end(), [block](auto &ptr) {
        return ptr.get() == block;
    });
}

Function::const_iterator Function::remove(BasicBlock *block) {
    auto it = position_of(block);
    if (it != m_blocks.end()) {
        m_blocks.erase(it);
    }
    return it;
}

} // namespace bamf
