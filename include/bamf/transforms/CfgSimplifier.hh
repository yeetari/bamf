#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct CfgSimplifier : public Pass {
    CfgSimplifier() : Pass("cfg-simplifier") {}

    void run_on(Function *function) override;
};

} // namespace bamf
