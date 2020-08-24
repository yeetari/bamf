#include <bamf/analyses/ControlFlowAnalysis.hh>

#include <cassert>

namespace bamf {

BasicBlock *ControlFlowAnalysis::entry() {
    assert(m_cfg.entry() == m_dom_tree.entry());
    return m_cfg.entry();
}

std::vector<BasicBlock *> ControlFlowAnalysis::preds(BasicBlock *block) {
    return m_cfg.preds_of(block);
}

std::vector<BasicBlock *> ControlFlowAnalysis::succs(BasicBlock *block) {
    return m_cfg.succs_of(block);
}

std::vector<BasicBlock *> ControlFlowAnalysis::tree_succs(BasicBlock *block) {
    return m_dom_tree.succs_of(block);
}

const std::unordered_set<BasicBlock *> &ControlFlowAnalysis::frontiers(BasicBlock *block) {
    return m_frontiers[block];
}

} // namespace bamf
