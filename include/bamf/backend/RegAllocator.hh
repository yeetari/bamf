#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct RegAllocator : public Pass {
    explicit RegAllocator(PassManager *manager) : Pass(manager, "reg-allocator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
