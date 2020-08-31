#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct MoveInserter : public Pass {
    explicit MoveInserter(PassManager *manager) : Pass(manager, "move-inserter") {}

    void run_on(Function *function) override;
};

} // namespace bamf
