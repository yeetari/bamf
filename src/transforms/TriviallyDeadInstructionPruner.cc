#include <bamf/transforms/TriviallyDeadInstructionPruner.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>

namespace bamf {

void TriviallyDeadInstructionPruner::run_on(Function *function) {
    std::vector<Instruction *> work_queue;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            work_queue.push_back(inst.get());
        }
    }

    while (!work_queue.empty()) {
        auto *inst = work_queue.back();
        work_queue.pop_back();

        // If an instruction has uses, it isn't dead
        if (!inst->uses().empty()) {
            continue;
        }

        // Dead stores are handled by DeadStorePruner
        if (inst->is<StoreInst>()) {
            continue;
        }

        // Returns are never explictly used, and therefore can never be trivially dead
        if (inst->is<RetInst>()) {
            continue;
        }

        inst->remove_from_parent();
    }
}

} // namespace bamf
