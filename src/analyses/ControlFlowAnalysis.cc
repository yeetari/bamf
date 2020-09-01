#include <bamf/analyses/ControlFlowAnalysis.hh>

#include <cassert>

namespace bamf {

BasicBlock *ControlFlowAnalysis::entry() {
    assert(m_cfg.entry() == m_dom_tree.entry());
    return m_cfg.entry();
}

std::vector<BasicBlock *> ControlFlowAnalysis::preds(BasicBlock *block) {
    return m_cfg.preds(block);
}

std::vector<BasicBlock *> ControlFlowAnalysis::succs(BasicBlock *block) {
    return m_cfg.succs(block);
}

bool ControlFlowAnalysis::dominates(Instruction *dominator, Instruction *dominatee) {
    auto *dominator_block = dominator->parent();
    auto *dominatee_block = dominatee->parent();
    if (dominator_block == dominatee_block) {
        return std::distance(dominator_block->position_of(dominator), dominatee_block->position_of(dominatee)) > 0;
    }
    return m_dom_tree.dominates(dominator_block, dominatee_block);
}

std::vector<BasicBlock *> ControlFlowAnalysis::tree_succs(BasicBlock *block) {
    return m_dom_tree.succs(block);
}

const std::unordered_set<BasicBlock *> &ControlFlowAnalysis::frontiers(BasicBlock *block) {
    return m_frontiers[block];
}

} // namespace bamf
