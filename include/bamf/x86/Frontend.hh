#pragma once

#include <bamf/ir/BasicBlock.hh>
#include <bamf/ir/Local.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>
#include <bamf/x86/Decoder.hh>

#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

namespace bamf::x86 {

class Frontend {
    Decoder *const m_decoder;

    BasicBlock *m_block{nullptr};
    std::unordered_map<std::size_t, Local> m_locals;
    std::vector<const Local *> m_stack;

    Local *local(std::size_t);
    Local *reg_local(Register);

    void translate_mov_reg_imm(Register, std::size_t);
    void translate_mov_reg_reg(Register, Register);
    void translate_push_reg(Register);
    void translate_pop_reg(Register);
    void translate_ret();

public:
    explicit Frontend(Decoder *decoder) : m_decoder(decoder) {}

    std::unique_ptr<BasicBlock> run();
};

} // namespace bamf::x86
