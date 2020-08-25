#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

class DecompilationContext;

class RegisterLocaliser : public Pass {
    const DecompilationContext &m_decomp_ctx;

public:
    RegisterLocaliser(PassManager *manager, const DecompilationContext &decomp_ctx)
        : Pass(manager, "register-localiser"), m_decomp_ctx(decomp_ctx) {}

    void run_on(Program *program) override;
};

} // namespace bamf
