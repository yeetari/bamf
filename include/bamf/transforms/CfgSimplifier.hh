#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct CfgSimplifier : public Pass {
    explicit CfgSimplifier(PassManager *manager) : Pass(manager, "cfg-simplifier") {}

    void build_usage(PassUsage *usage) override;
    void run_on(Function *function) override;
};

} // namespace bamf
