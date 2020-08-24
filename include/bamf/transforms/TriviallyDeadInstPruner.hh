#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct TriviallyDeadInstPruner : public Pass {
    explicit TriviallyDeadInstPruner(PassManager *manager) : Pass(manager, "trivially-dead-inst-pruner") {}

    void run_on(Function *function) override;
};

} // namespace bamf
