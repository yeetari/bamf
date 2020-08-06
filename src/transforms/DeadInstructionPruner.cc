#include <bamf/transforms/DeadInstructionPruner.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>

#include <unordered_set>

namespace bamf {

namespace {

void run(BasicBlock *block) {
    std::unordered_set<Instruction *> to_remove;
    bool changed = false;
    do {
        changed = false;
        for (auto &inst : *block) {
            // If an instruction has uses, it isn't dead
            if (!inst->uses().empty()) {
                continue;
            }

            // Dead stores are handled by DeadStorePruner
            if (inst->is<StoreInst>()) {
                continue;
            }

            if (inst->is<RetInst>()) {
                continue;
            }

            changed = true;
            to_remove.insert(inst.get());
        }

        for (auto *inst : to_remove) {
            block->remove(inst);
        }
    } while (changed);
}

} // namespace

void DeadInstructionPruner::run_on(Function *function) {
    for (auto &block : *function) {
        run(block.get());
    }
}

} // namespace bamf
