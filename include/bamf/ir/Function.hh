#pragma once

#include <bamf/ir/BasicBlock.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace bamf {

class Function {
    const std::string m_name;
    std::vector<std::unique_ptr<BasicBlock>> m_blocks;

public:
    BAMF_MAKE_ITERABLE(m_blocks)
    BAMF_MAKE_NON_COPYABLE(Function)
    BAMF_MAKE_NON_MOVABLE(Function)

    explicit Function(std::string name) : m_name(std::move(name)) {}
    ~Function() = default;

    BasicBlock *insert_block();
    const_iterator position_of(BasicBlock *block) const;
    const_iterator remove(BasicBlock *block);

    const std::string &name() const { return m_name; }
    BasicBlock *entry() { return m_blocks.front().get(); }
};

} // namespace bamf
