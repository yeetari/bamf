#include <bamf/transforms/CfgSimplifier.hh>

#include <bamf/analyses/ControlFlowAnalyser.hh>
#include <bamf/analyses/ControlFlowAnalysis.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/PassUsage.hh>
#include <bamf/pass/Statistic.hh>
#include <bamf/support/Stack.hh>

namespace bamf {

namespace {

bool run(Function *function, ControlFlowAnalysis *cfa, const Statistic &no_preds_pruned_count,
         const Statistic &single_branch_pruned_count) {
    bool changed = false;
    Stack<BasicBlock> remove_queue;
    for (auto &b : *function) {
        auto *block = b.get();
        // Remove blocks with no predecessors. This can happen after running, for example, the ConstantBranchEvaluator
        // pass where conditional branches (with two basic block destinations) are promoted into unconditional branches
        // (with only one basic block destination).
        if (cfa->preds(block).empty() && block != cfa->entry()) {
            // Removing the block from all PHIs.
            for (auto *user : block->users()) {
                if (auto *phi = user->as<PhiInst>()) {
                    phi->remove_incoming(block);
                }
            }
            remove_queue.push(block);
            changed = true;
            ++no_preds_pruned_count;
        }

        // Remove blocks with an unconditional branch as the only instruction.
        if (block->size() == 1) {
            auto *branch = block->terminator()->as<BranchInst>();
            if (branch == nullptr) {
                continue;
            }
            block->replace_all_uses_with(branch->dst());
            remove_queue.push(block);
            changed = true;
            ++single_branch_pruned_count;
        }
    }
    while (!remove_queue.empty()) {
        function->remove(remove_queue.pop());
    }
    return changed;
}

} // namespace

void CfgSimplifier::build_usage(PassUsage *usage) {
    usage->depends_on<ControlFlowAnalyser>();
}

void CfgSimplifier::run_on(Function *function) {
    bool changed = false;
    Statistic no_preds_pruned_count(m_logger, "Pruned {} blocks with no predecessors");
    Statistic single_branch_pruned_count(m_logger, "Pruned {} blocks with only a single unconditional branch");
    do {
        auto *cfa = m_manager->get<ControlFlowAnalysis>(function);
        changed = run(function, cfa, no_preds_pruned_count, single_branch_pruned_count);
    } while (changed);
}

} // namespace bamf
