#pragma once

#include <bamf/ir/Instruction.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <algorithm>
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
    ~BasicBlock() {
        for (auto &inst : m_instructions) {
            inst->replace_all_uses_with(nullptr);
        }
    }

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

    void remove(Instruction *inst) {
        auto it = std::find_if(m_instructions.begin(), m_instructions.end(), [inst](auto &ptr) {
            return ptr.get() == inst;
        });

        if (it != m_instructions.end()) {
            inst->replace_all_uses_with(nullptr);
            m_instructions.erase(it);
        }
    }
};

} // namespace bamf
