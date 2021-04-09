#include <bamf/analyses/DataFlowAnalyser.hh>

#include <bamf/analyses/DataFlowAnalysis.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Constant.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Instructions.hh>
#include <bamf/pass/PassManager.hh>

#include <bamf/graph/DotGraph.hh>
#include <iostream>
#include <unordered_set>

namespace bamf {

void DataFlowAnalyser::run_on(Function *function) {
    auto *dfa = m_manager->make<DataFlowAnalysis>(function);
    auto &dfg = dfa->m_dfg;
    auto *entry = new Constant(0);
    dfg.set_entry(entry);
    std::unordered_set<Value *> visited;
    for (auto &block : *function) {
        std::function<void(Value *)> recurse = [&](Value *value) {
            if (visited.contains(value)) {
                return;
            }
            visited.insert(value);
            for (auto *user : value->users()) {
                dfg.connect(value, user);
                recurse(user);
            }
        };
        for (auto &inst : *block) {
            dfg.connect(entry, inst.get());
            recurse(inst.get());
        }
    }

    DotGraph<Value> dot(dfg);
    auto dfs = dfg.run<DepthFirstSearch>();
    for (auto *vertex : dfs.pre_order()) {
        if (auto *binary = vertex->as<BinaryInst>()) {
            dot.label(vertex, "binary");
        } else if (auto *branch = vertex->as<BranchInst>()) {
            dot.label(vertex, "branch");
        } else if (auto *compare = vertex->as<CompareInst>()) {
            dot.label(vertex, "compare");
        } else if (auto *cond_branch = vertex->as<CondBranchInst>()) {
            dot.label(vertex, "cond_branch");
        } else if (auto *constant = vertex->as<Constant>()) {
            dot.label(vertex, "i64 " + std::to_string(constant->value()));
        } else if (auto *phi = vertex->as<PhiInst>()) {
            dot.label(vertex, "phi");
        } else if (auto *ret = vertex->as<RetInst>()) {
            dot.label(vertex, "ret");
        } else {
            m_logger.warn("sad");
        }
    }

    std::cout << dot.to_string() << std::endl;
}

} // namespace bamf
