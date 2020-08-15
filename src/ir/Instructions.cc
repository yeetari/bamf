#include <bamf/ir/Instructions.hh>

#include <algorithm>

namespace bamf {

void PhiInst::remove_incoming(BasicBlock *block) {
    if (!m_incoming.contains(block)) {
        return;
    }

    auto *value = m_incoming.at(block);
    if (value != nullptr) {
        value->remove_use(&m_incoming[block]);
        value->remove_user(this);
    }
    m_incoming.erase(block);
}

} // namespace bamf
