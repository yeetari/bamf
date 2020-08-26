#pragma once

#include <bamf/graph/Graph.hh>

#include <cassert>

namespace bamf {

template <typename V>
struct DominatorTree : public Graph<V> {
    bool dominates(const V *dominator, const V *dominatee);
    V *idom(const V *vertex);
};

// TODO: Add tests.
template <typename V>
bool DominatorTree<V>::dominates(const V *dominator, const V *dominatee) {
    if (dominator == dominatee) {
        return true;
    }

    for (const V *pred = dominatee; pred != nullptr && pred != this->entry(); pred = idom(pred)) {
        if (pred == dominator) {
            return true;
        }
    }
    return false;
}

template <typename V>
V *DominatorTree<V>::idom(const V *vertex) {
    assert(this->preds(vertex).size() == 1);
    return this->preds(vertex)[0];
}

} // namespace bamf
