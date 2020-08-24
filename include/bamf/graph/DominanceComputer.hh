#pragma once

#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/graph/DominatorTree.hh>
#include <bamf/graph/Graph.hh>

#include <algorithm>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

namespace bamf {

template <typename V>
class DominanceComputer {
    friend Graph<V>;

protected:
    using result = DominatorTree<V>;
    result run(Graph<V> *graph);
};

template <typename V>
typename DominanceComputer<V>::result DominanceComputer<V>::run(Graph<V> *graph) {
    // Build post order traversal.
    // TODO: Avoid copying of post order vector here.
    auto dfs = graph->template run<DepthFirstSearch>();
    auto order = dfs.post_order();

    // Build map of vertices to index in the post order traversal vector.
    std::unordered_map<V *, std::size_t> index_map;
    for (auto *vertex : order) {
        index_map.emplace(vertex, index_map.size());
    }

    // Transform into reverse post order.
    std::reverse(order.begin(), order.end());

    std::unordered_map<V *, V *> doms;
    doms.emplace(graph->entry(), graph->entry());
    auto intersect = [&](V *finger1, V *finger2) {
        while (index_map.at(finger1) != index_map.at(finger2)) {
            while (index_map.at(finger1) < index_map.at(finger2)) {
                finger1 = doms.at(finger1);
            }
            while (index_map.at(finger2) < index_map.at(finger1)) {
                finger2 = doms.at(finger2);
            }
        }
        return finger1;
    };

    // Remove graph entry from order.
    assert(order.front() == graph->entry());
    order.erase(order.begin());

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto *b : order) {
            V *new_idom = nullptr;
            for (auto *pred : graph->preds_of(b)) {
                if (!graph->preds_of(pred).empty() || pred == graph->entry()) {
                    new_idom = pred;
                    break;
                }
            }
            for (auto *p : graph->preds_of(b)) {
                if (p != new_idom && doms.contains(p)) {
                    new_idom = intersect(p, new_idom);
                }
            }

            changed = doms[b] == new_idom;
            doms[b] = new_idom;
        }
    }

    // Build idom tree.
    DominatorTree<V> tree;
    tree.set_entry(graph->entry());
    for (auto [vertex, dom] : doms) {
        // Ignore self-domination (non-strict).
        if (vertex != dom) {
            tree.template connect(dom, vertex);
        }
    }
    return std::move(tree);
}

} // namespace bamf
