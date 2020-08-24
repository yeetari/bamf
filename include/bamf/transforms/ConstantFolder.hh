#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct ConstantFolder : public Pass {
    explicit ConstantFolder(PassManager *manager) : Pass(manager, "constant-folder") {}

    void run_on(Function *function) override;
};

} // namespace bamf
