#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct DeadInstructionPruner : public Pass {
    DeadInstructionPruner() : Pass("Dead Instruction Pruner") {}

    void run_on(Function *function) override;
};

} // namespace bamf
