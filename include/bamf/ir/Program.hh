#pragma once

#include <bamf/graph/Graph.hh>
#include <bamf/ir/Function.hh>
#include <bamf/ir/GlobalVariable.hh>
#include <bamf/ir/Value.hh>
#include <bamf/support/Iterable.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <algorithm>
#include <cassert>
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

    // TODO: Move to source file.
    GlobalVariable *add_global() { return m_globals.emplace_back(new GlobalVariable).get(); }
    void remove_global(GlobalVariable *global) {
        assert(global->users().empty());
        auto it = std::find_if(m_globals.begin(), m_globals.end(), [global](auto &ptr) {
            return ptr.get() == global;
        });
        if (it != m_globals.end()) {
            m_globals.erase(it);
        }
    }

    template <typename... Args>
    Function *create_function(Args &&... args) {
        return m_call_graph.emplace(std::forward<Args>(args)...);
    }

    void set_main(Function *function) { m_call_graph.set_entry(function); }
    Function *main() { return m_call_graph.entry(); }

    const Graph<Function> &call_graph() const { return m_call_graph; }
    const std::vector<std::unique_ptr<GlobalVariable>> &globals() const { return m_globals; }
};

} // namespace bamf
