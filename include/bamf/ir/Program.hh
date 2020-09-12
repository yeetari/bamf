#pragma once

#include <bamf/graph/Graph.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/GlobalVariable.hh>
#include <bamf/support/Iterable.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <memory>
#include <vector>

namespace bamf {

class Program {
    Graph<Function> m_call_graph;
    std::vector<std::unique_ptr<GlobalVariable>> m_globals;

public:
    BAMF_MAKE_ITERABLE(m_call_graph)
    BAMF_MAKE_NON_COPYABLE(Program)
    BAMF_MAKE_NON_MOVABLE(Program)

    Program() = default;
    ~Program() = default;

    template <typename... Args>
    Function *create_function(Args &&... args);

    GlobalVariable *add_global();
    void remove_global(GlobalVariable *global);

    void set_main(Function *function) { m_call_graph.set_entry(function); }
    Function *main() { return m_call_graph.entry(); }

    const Graph<Function> &call_graph() const { return m_call_graph; }
    const std::vector<std::unique_ptr<GlobalVariable>> &globals() const { return m_globals; }
};

template <typename... Args>
Function *Program::create_function(Args &&... args) {
    return m_call_graph.emplace(std::forward<Args>(args)...);
}

} // namespace bamf
