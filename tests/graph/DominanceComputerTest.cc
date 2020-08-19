#include <bamf/graph/DominanceComputer.hh>

#include <gtest/gtest.h>

namespace bamf {

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

TEST(DominanceComputerTest, Test) {
    Graph<TestVertex> graph;
    auto *a = graph.emplace(1);
    auto *b = graph.emplace(2);
    auto *c = graph.emplace(3);
    auto *d = graph.emplace(4);
    auto *e = graph.emplace(5);
    auto *f = graph.emplace(6);
    graph.connect<TestEdge>(a, b);
    graph.connect<TestEdge>(b, c);
    graph.connect<TestEdge>(b, d);
    graph.connect<TestEdge>(b, f);
    graph.connect<TestEdge>(c, e);
    graph.connect<TestEdge>(d, e);
    graph.connect<TestEdge>(e, b);
    graph.set_entry(a);

    auto tree = graph.run<DominanceComputer>();
    ASSERT_EQ(tree.entry(), graph.entry());

    EXPECT_EQ(tree.preds_of(a).size(), 0);
    EXPECT_EQ(tree.succs_of(a).size(), 1);
    EXPECT_EQ(tree.succs_of(a)[0], b);

    EXPECT_EQ(tree.preds_of(b).size(), 1);
    EXPECT_EQ(tree.succs_of(b).size(), 4);
    EXPECT_EQ(tree.succs_of(b)[0], e);
    EXPECT_EQ(tree.succs_of(b)[1], c);
    EXPECT_EQ(tree.succs_of(b)[2], d);
    EXPECT_EQ(tree.succs_of(b)[3], f);
}

} // namespace

} // namespace bamf
