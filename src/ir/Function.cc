#include <bamf/ir/Function.hh>

namespace bamf {

void Function::dump() const {
    for (const auto &block : *this) {
        block->dump();
    }
}

} // namespace bamf
