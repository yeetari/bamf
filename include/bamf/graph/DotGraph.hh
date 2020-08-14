#pragma once

#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/graph/Graph.hh>

#include <sstream>
#include <string>
#include <unordered_map>

namespace bamf {

template <typename V>
class DotGraph {
    const Graph<V> &m_graph;

public:
    explicit DotGraph(const Graph<V> &graph) : m_graph(graph) {}

    std::string to_string() const;
};

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
        ss << "\t" << unique_id(vertex) << " [];\n";
        for (auto *edge : m_graph.edges_of(vertex)) {
            ss << "\t" << unique_id(edge->src()) << " -> " << unique_id(edge->dst()) << "[];\n";
        }
    }
    ss << "}\n";
    return ss.str();
}

} // namespace bamf
