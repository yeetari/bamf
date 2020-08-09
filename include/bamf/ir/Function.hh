#pragma once

#include <bamf/graph/Graph.hh>
#include <bamf/ir/BasicBlock.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <string>
#include <utility>
#include <vector>

namespace bamf {

class Function {
    const std::string m_name;
    Graph<BasicBlock> m_cfg;

public:
    BAMF_MAKE_ITERABLE(m_cfg)
    BAMF_MAKE_NON_COPYABLE(Function)
    BAMF_MAKE_NON_MOVABLE(Function)

    explicit Function(std::string name) : m_name(std::move(name)) {}
    ~Function() = default;

    BasicBlock *insert_block() {
        auto *block = m_cfg.emplace();
        block->set_parent(this);
        return block;
    }

    void set_entry(BasicBlock *block) { m_cfg.set_entry(block); }
    BasicBlock *entry() { return m_cfg.entry(); }

    const std::string &name() const { return m_name; }
};

} // namespace bamf
