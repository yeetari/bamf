#pragma once

#include <bamf/graph/Graph.hh>

namespace bamf {

template <typename V>
struct DominatorTree : public Graph<V> {
    V *idom(const V *vertex);
};

template <typename V>
V *DominatorTree<V>::idom(const V *vertex) {
    return this->preds_of(vertex)[0];
}

} // namespace bamf
