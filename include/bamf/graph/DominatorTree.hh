#pragma once

#include <bamf/graph/Graph.hh>

#include <cassert>

namespace bamf {

template <typename V>
struct DominatorTree : public Graph<V> {
    V *idom(const V *vertex);
};

template <typename V>
V *DominatorTree<V>::idom(const V *vertex) {
    assert(this->preds(vertex).size() == 1);
    return this->preds(vertex)[0];
}

} // namespace bamf
