#pragma once

#include <bamf/graph/Edge.hh>
#include <bamf/support/Iterator.hh>
#include <bamf/support/NonCopyable.hh>
#include <bamf/support/NonMovable.hh>

#include <algorithm>
#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>

namespace bamf {

template <typename V>
class Graph {
    std::unordered_map<const V *, std::vector<std::unique_ptr<Edge<V>>>> m_preds;
    std::unordered_map<const V *, std::vector<std::unique_ptr<Edge<V>>>> m_succs;
    std::vector<std::unique_ptr<V>> m_vertices;
    const V *m_entry{nullptr};

public:
    BAMF_MAKE_ITERABLE(m_vertices)
    BAMF_MAKE_NON_COPYABLE(Graph)
    BAMF_MAKE_NON_MOVABLE(Graph)

    Graph() = default;
    ~Graph() = default;

    template <typename E, typename... Args>
    void connect(V *src, V *dst, Args &&... args);

    template <typename... Args>
    V *emplace(Args &&... args);
    void remove(const V *vertex);

    std::vector<V *> preds_of(const V *vertex);
    std::vector<V *> succs_of(const V *vertex);

    std::size_t size() const;
};

template <typename V>
template <typename E, typename... Args>
void Graph<V>::connect(V *src, V *dst, Args &&... args) {
    m_preds[dst].emplace_back(new E(src, dst, std::forward<Args>(args)...));
    m_succs[src].emplace_back(new E(src, dst, std::forward<Args>(args)...));
}

template <typename V>
template <typename... Args>
V *Graph<V>::emplace(Args &&... args) {
    return m_vertices.emplace_back(new V(std::forward<Args>(args)...)).get();
}

template <typename V>
void Graph<V>::remove(const V *vertex) {
    m_preds.erase(vertex);
    m_succs.erase(vertex);

    auto it = std::find_if(m_vertices.begin(), m_vertices.end(), [&](const auto &ptr) {
        return vertex == ptr.get();
    });

    if (it != m_vertices.end()) {
        m_vertices.erase(it);
    }
}

template <typename V>
std::vector<V *> Graph<V>::preds_of(const V *vertex) {
    std::vector<V *> ret;
    for (const auto &pred : m_preds[vertex]) {
        ret.push_back(pred->src());
    }
    return ret;
}

template <typename V>
std::vector<V *> Graph<V>::succs_of(const V *vertex) {
    std::vector<V *> ret;
    for (const auto &succ : m_succs[vertex]) {
        ret.push_back(succ->dst());
    }
    return ret;
}

template <typename V>
std::size_t Graph<V>::size() const {
    return m_vertices.size();
}

} // namespace bamf
