#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct CfgSimplifier : public Pass {
    explicit CfgSimplifier(PassManager *manager) : Pass(manager, "cfg-simplifier") {}

    void run_on(Function *function) override;
};

} // namespace bamf
