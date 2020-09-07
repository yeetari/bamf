#pragma once

#include <bamf/graph/DominatorTree.hh>
#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/pass/PassResult.hh>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bamf {

class ControlFlowAnalyser;

class ControlFlowAnalysis : public PassResult {
    friend ControlFlowAnalyser;

private:
    Graph<BasicBlock> m_cfg;
    DominatorTree<BasicBlock> m_dom_tree;
    std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> m_frontiers;

public:
    BasicBlock *entry();
    std::vector<BasicBlock *> preds(BasicBlock *block);
    std::vector<BasicBlock *> succs(BasicBlock *block);

    bool dominates(Instruction *dominator, Instruction *dominatee);
    std::vector<BasicBlock *> tree_succs(BasicBlock *block);
    const std::unordered_set<BasicBlock *> &frontiers(BasicBlock *block);

    const Graph<BasicBlock> &cfg() const { return m_cfg; }
};

} // namespace bamf
