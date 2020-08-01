#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct Dumper : public Pass {
    Dumper() : Pass("Dumper") {}

    void run_on(Function *function) override;
};

} // namespace bamf
