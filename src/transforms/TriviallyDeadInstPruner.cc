#include <bamf/transforms/TriviallyDeadInstPruner.hh>

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/Statistic.hh>

namespace bamf {

void TriviallyDeadInstPruner::run_on(Function *function) {
    std::vector<Instruction *> work_queue;
    for (auto &block : *function) {
        for (auto &inst : *block) {
            work_queue.push_back(inst.get());
        }
    }

    Statistic pruned_count(m_logger, "Pruned {} trivially dead instructions");
    while (!work_queue.empty()) {
        auto *inst = work_queue.back();
        work_queue.pop_back();

        // If an instruction has users, it isn't dead.
        if (!inst->users().empty()) {
            continue;
        }

        // Dead stores are promoted by AllocPromoter.
        if (inst->is<StoreInst>()) {
            continue;
        }

        // Terminators are never explictly used, and therefore can never be trivially dead.
        if (inst->is<BranchInst>() || inst->is<CondBranchInst>() || inst->is<RetInst>()) {
            continue;
        }

        inst->remove_from_parent();
        ++pruned_count;
    }
}

} // namespace bamf
