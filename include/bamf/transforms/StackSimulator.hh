#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

class DecompilationContext;

class StackSimulator : public Pass {
    const DecompilationContext &m_decomp_ctx;

public:
    explicit StackSimulator(const DecompilationContext &decomp_ctx)
        : Pass("stack-simulator"), m_decomp_ctx(decomp_ctx) {}

    void run_on(Function *function) override;
};

} // namespace bamf
