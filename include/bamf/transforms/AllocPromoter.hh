#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct AllocPromoter : public Pass {
    AllocPromoter() : Pass("alloc-promoter") {}

    void run_on(Function *function) override;
};

} // namespace bamf
