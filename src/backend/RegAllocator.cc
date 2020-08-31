#include <bamf/backend/RegAllocator.hh>

#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>

#include <cassert>
#include <unordered_map>

namespace bamf {

void RegAllocator::run_on(Function *function) {
    std::unordered_map<VirtReg *, PhysReg *> map;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            auto *move = inst->as<MoveInst>();
            if (move == nullptr) {
                continue;
            }
            auto *virt = move->dst()->as<VirtReg>();
            if (virt == nullptr) {
                continue;
            }
            if (!map.contains(virt)) {
                map.emplace(virt, new PhysReg(map.size()));
            }
            virt->replace_all_uses_with(map.at(virt));
        }
    }
}

} // namespace bamf
