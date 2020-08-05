#pragma once

#include <bamf/graph/Graph.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/Value.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <memory>
#include <vector>

namespace bamf {

class Program {
    Graph<Function> m_call_graph;
    std::vector<std::unique_ptr<Value>> m_globals;

public:
    BAMF_MAKE_ITERABLE(m_call_graph)
    BAMF_MAKE_NON_COPYABLE(Program)
    BAMF_MAKE_NON_MOVABLE(Program)

    Program() = default;
    ~Program() = default;

    Value *add_global() {
        return m_globals.emplace_back(new Value).get();
    }

    template <typename... Args>
    Function *create_function(Args &&... args) {
        return m_call_graph.emplace(std::forward<Args>(args)...);
    }

    void set_main(Function *function) { m_call_graph.set_entry(function); }
    Function *main() { return m_call_graph.entry(); }
};

} // namespace bamf
