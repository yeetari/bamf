#pragma once

#include <bamf/ir/Instruction.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <concepts>
#include <memory>
#include <utility>
#include <vector>

namespace bamf {

class BasicBlock {
    std::vector<std::unique_ptr<Instruction>> m_instructions;

public:
    BAMF_MAKE_ITERABLE(m_instructions)
    BAMF_MAKE_NON_COPYABLE(BasicBlock)
    BAMF_MAKE_NON_MOVABLE(BasicBlock)

    BasicBlock() = default;
    ~BasicBlock() = default;

    template <typename Inst, typename... Args>
    Inst *insert(Args &&... args) requires std::derived_from<Inst, Instruction> {
        return static_cast<Inst *>(m_instructions.emplace_back(new Inst(std::forward<Args>(args)...)).get());
    }
};

} // namespace bamf
