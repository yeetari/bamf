#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct AllocPromoter : public Pass {
    explicit AllocPromoter(PassManager *manager) : Pass(manager, "alloc-promoter") {}

    void build_usage(PassUsage *usage) override;
    void run_on(Function *function) override;
};

} // namespace bamf
