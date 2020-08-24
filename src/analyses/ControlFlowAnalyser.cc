#include <bamf/analyses/ControlFlowAnalyser.hh>

#include <bamf/analyses/ControlFlowAnalysis.hh>
#include <bamf/graph/DominanceComputer.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/PassManager.hh>

namespace bamf {

void ControlFlowAnalyser::run_on(Function *function) {
    auto *cfa = m_manager->make<ControlFlowAnalysis>(function);
    auto &cfg = cfa->m_cfg;

    // Build control flow graph.
    cfg.set_entry(function->entry());
    for (auto &block : *function) {
        for (auto &inst : *block) {
            if (auto *branch = inst->as<BranchInst>()) {
                cfg.connect(branch->parent(), branch->dst());
            } else if (auto *cond_branch = inst->as<CondBranchInst>()) {
                cfg.connect(cond_branch->parent(), cond_branch->false_dst());
                cfg.connect(cond_branch->parent(), cond_branch->true_dst());
            }
        }
    }

    // Build dominator tree and dominance frontiers.
    // TODO: Move dominance frontiers into graph lib.
    cfa->m_dom_tree = cfg.run<DominanceComputer>();
    auto &tree = cfa->m_dom_tree;
    auto &frontiers = cfa->m_frontiers;
    auto tree_dfs = tree.run<DepthFirstSearch>();
    for (auto *post_idom : tree_dfs.post_order()) {
        //
        for (auto *succ : cfg.succs(post_idom)) {
            if (tree.idom(succ) != post_idom) {
                frontiers[post_idom].insert(succ);
            }
        }

        //
        for (auto *f : tree.succs(post_idom)) {
            for (auto *ff : frontiers[f]) {
                if (tree.idom(ff) != post_idom) {
                    frontiers[post_idom].insert(ff);
                }
            }
        }
    }
}

} // namespace bamf
