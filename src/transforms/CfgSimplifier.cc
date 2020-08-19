#include <bamf/transforms/CfgSimplifier.hh>

#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/Statistic.hh>
#include <bamf/support/Stack.hh>

namespace bamf {

namespace {

bool run(Function *function, const Statistic &no_preds_pruned_count) {
    // Build control flow graph.
    // TODO: Move this into an analysis pass.
    Graph<BasicBlock> cfg;
    cfg.set_entry(function->entry());
    for (auto &block : *function) {
        for (auto &inst : *block) {
            // TODO: Make Edge<V> the default template arg for connect.
            if (auto *branch = inst->as<BranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(branch->parent(), branch->dst());
            } else if (auto *cond_branch = inst->as<CondBranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->false_dst());
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->true_dst());
            }
        }
    }

    bool changed = false;
    Stack<BasicBlock> remove_queue;
    for (auto &b : *function) {
        auto *block = b.get();
        // Remove blocks with no predecessors. This can happen after running, for example, the ConstantBranchEvaluator
        // pass where conditional branches (with two basic block destinations) are promoted into unconditional branches
        // (with only one basic block destination).
        if (cfg.preds_of(block).empty() && block != cfg.entry()) {
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
    }
    while (!remove_queue.empty()) {
        function->remove(remove_queue.pop());
    }
    return changed;
}

} // namespace

void CfgSimplifier::run_on(Function *function) {
    bool changed = false;
    Statistic no_preds_pruned_count(m_logger, "Pruned {} blocks with no predecessors");
    do {
        changed = run(function, no_preds_pruned_count);
    } while (changed);
}

} // namespace bamf
