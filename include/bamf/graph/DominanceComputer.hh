#pragma once

#include <bamf/graph/DepthFirstSearch.hh>
#include <bamf/graph/DominatorTree.hh>
#include <bamf/graph/Graph.hh>

#include <algorithm>
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
    // Build post order
    auto dfs = graph->template run<DepthFirstSearch>();
    auto order = dfs.post_order();

    // Build post order index map
    std::unordered_map<V *, std::size_t> index_map;
    for (auto *vertex : order) {
        index_map.emplace(vertex, index_map.size());
    }

    // Transform into reverse post order
    std::reverse(order.begin(), order.end());

    std::unordered_map<V *, V *> doms;
    doms.emplace(graph->entry(), graph->entry());
    auto intersect = [&](V *b1, V *b2) {
        auto *finger1 = b1;
        auto *finger2 = b2;
        while (index_map[finger1] != index_map[finger2]) {
            while (index_map[finger1] < index_map[finger2]) {
                finger1 = doms[finger1];
            }
            while (index_map[finger2] < index_map[finger1]) {
                finger2 = doms[finger2];
            }
        }
        return finger1;
    };

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto *b : order) {
            if (b == graph->entry()) {
                continue;
            }

            auto *new_idom = graph->preds_of(b)[0];
            for (auto *p : graph->preds_of(b)) {
                if (p != new_idom && doms.contains(p)) {
                    new_idom = intersect(p, new_idom);
                }
            }

            if (doms[b] != new_idom) {
                doms[b] = new_idom;
                changed = true;
            }
        }
    }

    DominatorTree<V> tree;
    tree.set_entry(graph->entry());
    for (auto [vertex, dom] : doms) {
        // Ignore self-domination (non-strict)
        if (vertex != dom) {
            tree.template connect(dom, vertex);
        }
    }
    return std::move(tree);
}

} // namespace bamf
