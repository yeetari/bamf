#pragma once

#include <bamf/core/DecompilationContext.hh>
#include <bamf/ir/GlobalVariable.hh>
#include <bamf/ir/Program.hh>
#include <bamf/ir/Value.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>
#include <bamf/x86/Decoder.hh>

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace bamf::x86 {

class Frontend {
    Decoder *const m_decoder;
    DecompilationContext *const m_decomp_ctx;
    Program *m_program{nullptr};
    Function *m_function{nullptr};
    BasicBlock *m_block{nullptr};

    std::unordered_map<Register, GlobalVariable *> m_phys_regs;

    Value *phys_dst(Register);
    Value *phys_src(Register);

    void translate_mov(const Operand &dst, const Operand &src);
    void translate_pop(const Operand &dst);
    void translate_push(const Operand &src);
    void translate_ret();

public:
    Frontend(Decoder *decoder, DecompilationContext *decomp_ctx) : m_decoder(decoder), m_decomp_ctx(decomp_ctx) {}

    std::unique_ptr<Program> run();
};

} // namespace bamf::x86
