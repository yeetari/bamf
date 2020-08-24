#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct ControlFlowAnalyser : public Pass {
    explicit ControlFlowAnalyser(PassManager *manager) : Pass(manager, "cf-analyser") {}

    void run_on(Function *function) override;
};

} // namespace bamf
