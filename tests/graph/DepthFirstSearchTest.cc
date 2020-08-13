#include <bamf/graph/DepthFirstSearch.hh>

#include <gtest/gtest.h>

using namespace bamf;

namespace {

class TestVertex {
    int m_num;

public:
    constexpr explicit TestVertex(int num) : m_num(num) {}

    int num() const { return m_num; }
};

struct TestEdge : public Edge<TestVertex> {
    constexpr TestEdge(TestVertex *src, TestVertex *dst) : Edge(src, dst) {}
};

TEST(DepthFirstSearchTest, Diamond) {
    Graph<TestVertex> graph;
    auto *a = graph.emplace(0);
    auto *b = graph.emplace(1);
    auto *c = graph.emplace(2);
    auto *d = graph.emplace(3);
    graph.connect<TestEdge>(a, b);
    graph.connect<TestEdge>(a, c);
    graph.connect<TestEdge>(b, d);
    graph.connect<TestEdge>(c, d);
    graph.set_entry(a);

    auto dfs = graph.run<DepthFirstSearch>();
    const auto &pre_order = dfs.pre_order();
    const auto &post_order = dfs.post_order();
    const std::vector<TestVertex *> pre_order_expected = {a, b, d, c};
    const std::vector<TestVertex *> post_order_expected = {d, b, c, a};
    EXPECT_EQ(pre_order, pre_order_expected);
    EXPECT_EQ(post_order, post_order_expected);
}

} // namespace
