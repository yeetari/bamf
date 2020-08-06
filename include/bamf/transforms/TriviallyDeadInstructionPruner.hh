#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct TriviallyDeadInstructionPruner : public Pass {
    TriviallyDeadInstructionPruner() : Pass("tdip") {}

    void run_on(Function *function) override;
};

} // namespace bamf
