#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

struct SsaTranslator : public Pass {
    SsaTranslator() : Pass("ssa-translator") {}

    void run_on(Function *function) override;
};

} // namespace bamf
