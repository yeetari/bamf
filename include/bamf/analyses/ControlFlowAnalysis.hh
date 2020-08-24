#pragma once

#include <bamf/graph/DominatorTree.hh>
#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/pass/Analysis.hh>

#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace bamf {

class ControlFlowAnalyser;

class ControlFlowAnalysis : public Analysis {
    friend ControlFlowAnalyser;

private:
    Graph<BasicBlock> m_cfg;
    DominatorTree<BasicBlock> m_dom_tree;
    std::unordered_map<BasicBlock *, std::unordered_set<BasicBlock *>> m_frontiers;

public:
    BasicBlock *entry();
    std::vector<BasicBlock *> preds(BasicBlock *block);
    std::vector<BasicBlock *> succs(BasicBlock *block);
    std::vector<BasicBlock *> tree_succs(BasicBlock *block);
    const std::unordered_set<BasicBlock *> &frontiers(BasicBlock *block);
};

} // namespace bamf
