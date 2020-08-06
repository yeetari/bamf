#include <bamf/ir/Instruction.hh>

#include <bamf/ir/BasicBlock.hh>

#include <cassert>

namespace bamf {

void Instruction::remove_from_parent() {
    assert(has_parent());
    m_parent->remove(this);
}

} // namespace bamf
