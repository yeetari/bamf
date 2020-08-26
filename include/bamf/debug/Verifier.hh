#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct Verifier : public Pass {
    explicit Verifier(PassManager *manager) : Pass(manager, "verifier") {}

    void build_usage(PassUsage *usage) override;
    void run_on(Function *function) override;
};

} // namespace bamf
