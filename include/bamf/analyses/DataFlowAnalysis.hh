#pragma once

#include <bamf/graph/Graph.hh>
#include <bamf/ir/Value.hh>
#include <bamf/pass/PassResult.hh>

namespace bamf {

class DataFlowAnalyser;

class DataFlowAnalysis : public PassResult {
    friend DataFlowAnalyser;

private:
    Graph<Value> m_dfg;
};

} // namespace bamf
