#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct DeadStorePruner : public Pass {
    DeadStorePruner() : Pass("Dead Store Pruner") {}

    void run_on(Function *function) override;
};

} // namespace bamf
