#pragma once

#include <bamf/ir/Function.hh>
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
    std::vector<Local *> m_stack;

    Local &local(std::size_t);
    Local &reg_local(Register);

    void translate_mov(const Operand &dst, const Operand &src);
    void translate_pop(const Operand &dst);
    void translate_push(const Operand &src);
    void translate_ret();

public:
    explicit Frontend(Decoder *decoder) : m_decoder(decoder) {}

    std::unique_ptr<Function> run();
};

} // namespace bamf::x86
