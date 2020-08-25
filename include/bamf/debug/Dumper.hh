#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct Dumper : public Pass {
    explicit Dumper(PassManager *manager) : Pass(manager, "dumper") {}

    void run_on(Function *function) override;
};

} // namespace bamf
