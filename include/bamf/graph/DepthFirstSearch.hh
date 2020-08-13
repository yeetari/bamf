#pragma once

#include <bamf/graph/Graph.hh>

#include <unordered_map>
#include <vector>

namespace bamf {

template <typename V>
class DepthFirstSearch {
    friend Graph<V>;

private:
    enum class State {
        Unexplored,
        Exploring,
        Explored,
    };

    std::unordered_map<V *, State> m_state;
    std::vector<V *> m_pre_order;
    std::vector<V *> m_post_order;

    void dfs(Graph<V> *graph, V *vertex);

protected:
    using result = DepthFirstSearch<V>;
    result run(Graph<V> *graph);

public:
    const std::vector<V *> &pre_order() const { return m_pre_order; }
    const std::vector<V *> &post_order() const { return m_post_order; }
};

template <typename V>
void DepthFirstSearch<V>::dfs(Graph<V> *graph, V *vertex) {
    m_pre_order.push_back(vertex);
    for (auto *succ : graph->succs_of(vertex)) {
        if (m_state[succ] != State::Unexplored) {
            continue;
        }
        m_state[succ] = State::Exploring;
        dfs(graph, succ);
    }
    m_state[vertex] = State::Explored;
    m_post_order.push_back(vertex);
}

template <typename V>
typename DepthFirstSearch<V>::result DepthFirstSearch<V>::run(Graph<V> *graph) {
    for (auto &vertex : *graph) {
        m_state[vertex.get()] = State::Unexplored;
    }
    m_state[graph->entry()] = State::Exploring;
    dfs(graph, graph->entry());
    return std::move(*this);
}

} // namespace bamf
