#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct TriviallyDeadInstructionPruner : public Pass {
    TriviallyDeadInstructionPruner() : Pass("Trivially Dead Instruction Pruner") {}

    void run_on(Function *function) override;
};

} // namespace bamf
