#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf::x86 {

struct Backend : public Pass {
    explicit Backend(PassManager *manager) : Pass(manager, "x86-backend") {}

    void build_usage(PassUsage *usage) override;
    void run_on(Program *program) override;
    void run_on(Function *function) override;
};

} // namespace bamf::x86
