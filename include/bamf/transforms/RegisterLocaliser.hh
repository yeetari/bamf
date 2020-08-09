#pragma once

#include <bamf/pass/Pass.hh>

namespace bamf {

class DecompilationContext;

class RegisterLocaliser : public Pass {
    const DecompilationContext &m_decomp_ctx;

public:
    explicit RegisterLocaliser(const DecompilationContext &decomp_ctx)
        : Pass("register-localiser"), m_decomp_ctx(decomp_ctx) {}

    void run_on(Program *program) override;
};

} // namespace bamf
