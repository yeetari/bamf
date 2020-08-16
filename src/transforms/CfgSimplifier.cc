#include <bamf/transforms/CfgSimplifier.hh>

#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instruction.hh>
#include <bamf/ir/Instructions.hh>

namespace bamf {

namespace {

bool run(Function *function, int *no_preds_pruned_count) {
    // Build CFG
    // TODO: Move this into an analysis pass
    Graph<BasicBlock> cfg;
    cfg.set_entry(function->entry());
    for (auto &block : *function) {
        for (auto &inst : *block) {
            // TODO: Make Edge<V> the default template arg for connect
            if (auto *branch = inst->as<BranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(branch->parent(), branch->dst());
            } else if (auto *cond_branch = inst->as<CondBranchInst>()) {
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->false_dst());
                cfg.connect<Edge<BasicBlock>>(cond_branch->parent(), cond_branch->true_dst());
            }
        }
    }

    bool changed = false;
    for (auto &block : *function) {
        // Remove blocks with no predecessors. This can happen after running, for example, the ConstantBranchEvaluator
        // pass where conditional branches (with two basic block destinations) are promoted into unconditional branches
        // (with only one basic block destination).
        if (cfg.preds_of(block.get()).empty() && block.get() != cfg.entry()) {
            // Removing the block from all PHIs
            // TODO: Store the PHI users inside the basic block
            for (auto &b : *function) {
                for (auto &inst : *b) {
                    if (auto *phi = inst->as<PhiInst>()) {
                        phi->remove_incoming(block.get());
                    }
                }
            }
            function->remove(block.get());
            changed = true;
            (*no_preds_pruned_count)++;
        }
    }
    return changed;
}

} // namespace

void CfgSimplifier::run_on(Function *function) {
    int no_preds_pruned_count = 0;
    bool changed = false;
    do {
        changed = run(function, &no_preds_pruned_count);
    } while (changed);
    m_logger.trace("Pruned {} blocks with no predecessors", no_preds_pruned_count);
}

} // namespace bamf
