#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct DataFlowAnalyser : public Pass {
    explicit DataFlowAnalyser(PassManager *manager) : Pass(manager, "df-analyser") {}

    void run_on(Function *function) override;
};

} // namespace bamf
