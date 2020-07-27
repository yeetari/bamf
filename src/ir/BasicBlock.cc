#include <bamf/ir/BasicBlock.hh>

namespace bamf {

void BasicBlock::dump() const {
    for (const auto &stmt : *this) {
        stmt->dump();
    }
}

} // namespace bamf
