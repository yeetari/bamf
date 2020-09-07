#pragma once

#include <bamf/graph/Edge.hh>

namespace bamf {

class TestVertex {
    int m_num;

public:
    constexpr explicit TestVertex(int num) : m_num(num) {}

    int num() const { return m_num; }
};

struct TestEdge : public Edge<TestVertex> {
    constexpr TestEdge(TestVertex *src, TestVertex *dst) : Edge(src, dst) {}
};

} // namespace bamf
