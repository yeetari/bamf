#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct ConstantBranchEvaluator : public Pass {
    ConstantBranchEvaluator() : Pass("constant-branch-evaluator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
