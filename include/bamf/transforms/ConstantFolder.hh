#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct ConstantFolder : public Pass {
    ConstantFolder() : Pass("constant-folder") {}

    void run_on(Function *function) override;
};

} // namespace bamf
