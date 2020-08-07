#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct StackSimulator : public Pass {
    StackSimulator() : Pass("stack-simulator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
