#pragma once

#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/graph/Graph.hh>

#include <cassert>
#include <sstream>
#include <string>
#include <unordered_map>

namespace bamf {

template <typename V>
class DotGraph {
    const Graph<V> &m_graph;
    std::unordered_map<const V *, std::string> m_labels;

public:
    explicit DotGraph(const Graph<V> &graph) : m_graph(graph) {}

    void label(const V *vertex, std::string label);
    std::string to_string() const;
};

template <typename V>
void DotGraph<V>::label(const V *vertex, std::string label) {
    assert(!m_labels.contains(vertex));
    m_labels.emplace(vertex, label);
}

template <typename V>
std::string DotGraph<V>::to_string() const {
    std::unordered_map<const V *, std::size_t> map;
    auto unique_id = [&](const V *vertex) {
        if (!map.contains(vertex)) {
            map.emplace(vertex, map.size());
        }
        return map.at(vertex);
    };

    std::stringstream ss;
    ss << "digraph {\n";
    ss << "\tnode [shape = box];\n";
    auto dfs = m_graph.template run<DepthFirstSearch>();
    for (auto *vertex : dfs.pre_order()) {
        ss << "\t" << unique_id(vertex) << " [";
        if (m_labels.contains(vertex)) {
            ss << "label=\"";
            ss << m_labels.at(vertex);
            ss << "\"";
        }
        ss << "];\n";
        for (auto *edge : m_graph.edges(vertex)) {
            ss << "\t" << unique_id(edge->src()) << " -> " << unique_id(edge->dst()) << "[];\n";
        }
    }
    ss << "}\n";
    return ss.str();
}

} // namespace bamf
