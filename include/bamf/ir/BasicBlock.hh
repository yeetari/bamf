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

class Function;

class BasicBlock {
    Function *m_parent{nullptr};
    std::vector<std::unique_ptr<Instruction>> m_instructions;

public:
    // TODO: Make custom iterator to iterate through `Instruction *` (rather than `std::unique_ptr<Instruction>`)
    BAMF_MAKE_ITERABLE(m_instructions)
    BAMF_MAKE_NON_COPYABLE(BasicBlock)
    BAMF_MAKE_NON_MOVABLE(BasicBlock)

    BasicBlock() = default;
    ~BasicBlock();

    template <typename Inst, typename... Args>
    Inst *insert(const_iterator position, Args &&... args) requires std::derived_from<Inst, Instruction> {
        auto *inst = new Inst(std::forward<Args>(args)...);
        inst->set_parent(this);
        m_instructions.emplace(position, inst);
        return inst;
    }

    template <typename Inst, typename... Args>
    Inst *prepend(Args &&... args) requires std::derived_from<Inst, Instruction> {
        return insert<Inst>(m_instructions.begin(), std::forward<Args>(args)...);
    }

    template <typename Inst, typename... Args>
    Inst *append(Args &&... args) requires std::derived_from<Inst, Instruction> {
        return insert<Inst>(m_instructions.end(), std::forward<Args>(args)...);
    }

    const_iterator position_of(Instruction *inst) const;
    const_iterator remove(Instruction *inst);

    void set_parent(Function *parent) { m_parent = parent; }
    Function *parent() const { return m_parent; }
};

} // namespace bamf
