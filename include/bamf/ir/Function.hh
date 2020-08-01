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

    void set_entry(BasicBlock *block) { m_cfg.set_entry(block); }

    BasicBlock *insert_block() { return m_cfg.emplace(); }
    BasicBlock *entry() { return m_cfg.entry(); }
};

} // namespace bamf
