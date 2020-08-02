#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct ConstantPropagator : public Pass {
    ConstantPropagator() : Pass("Constant Propagator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
