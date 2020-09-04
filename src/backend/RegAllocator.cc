#include <bamf/backend/RegAllocator.hh>

#include <bamf/backend/BackendInstructions.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>

#include <cassert>
#include <unordered_map>
#include <vector>

namespace bamf {

void RegAllocator::run_on(Function *function) {
    std::vector<Instruction *> work_queue;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            work_queue.push_back(inst.get());
        }
    }

    std::unordered_map<VirtReg *, Constraint> constraints;
    std::unordered_map<VirtReg *, Value *> map;
    for (auto *inst : work_queue) {
        if (auto *constraint = inst->as<ConstraintInst>()) {
            assert(!constraints.contains(constraint->reg()));
            constraints.emplace(constraint->reg(), constraint->constraint());
            continue;
        }

        auto *move = inst->as<MoveInst>();
        if (move == nullptr) {
            continue;
        }
        auto *virt = move->dst()->as<VirtReg>();
        if (virt == nullptr) {
            continue;
        }
        if (!map.contains(virt)) {
            Constraint constraint = constraints[virt];
            switch (constraint) {
            case Constraint::ReturnValue:
                // If the virtual register is a return value, always assign it to the 0th physical register (rax on
                // x86).
                map.emplace(virt, new PhysReg(0));
                break;
            default:
                // Else just spill the virtual register onto the stack using normal AllocInsts.
                auto *alloc = function->entry()->prepend<AllocInst>();
                map.emplace(virt, alloc);
                break;
            }
        }
        virt->replace_all_uses_with(map.at(virt));
    }
}

} // namespace bamf
