#include <bamf/backend/RegAllocator.hh>

#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>

namespace bamf {

void RegAllocator::run_on(Function *function) {
    for (auto &block : *function) {
        for (auto &inst : *block) {
            auto *move = inst->as<MoveInst>();
            if (move == nullptr) {
                continue;
            }
            // Replace virtual register with physical register 0 (rax on x86).
            move->set_dst(0);
        }
    }
}

} // namespace bamf
