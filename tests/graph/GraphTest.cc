#include <bamf/graph/Graph.hh>

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

TEST(GraphTest, Iterator) {
    Graph<TestVertex> graph;
    for (int i = 0; i < 10; i++) {
        graph.emplace(i);
    }

    int i = 0;
    for (const auto &vertex : graph) {
        EXPECT_EQ(vertex->num(), i++);
    }
    EXPECT_EQ(i, 10);
}

TEST(GraphTest, PredSucc) {
    Graph<TestVertex> graph;
    auto *a = graph.emplace(0);
    auto *b = graph.emplace(1);
    auto *c = graph.emplace(2);
    auto *d = graph.emplace(3);
    graph.connect<TestEdge>(a, b);
    graph.connect<TestEdge>(a, c);
    graph.connect<TestEdge>(b, d);
    graph.connect<TestEdge>(c, d);

    EXPECT_EQ(graph.preds_of(a).size(), 0);
    EXPECT_EQ(graph.succs_of(a).size(), 2);
    EXPECT_EQ(graph.succs_of(a)[0], b);
    EXPECT_EQ(graph.succs_of(a)[1], c);

    EXPECT_EQ(graph.preds_of(b).size(), 1);
    EXPECT_EQ(graph.preds_of(b)[0], a);
    EXPECT_EQ(graph.succs_of(b).size(), 1);
    EXPECT_EQ(graph.succs_of(b)[0], d);

    EXPECT_EQ(graph.preds_of(c).size(), 1);
    EXPECT_EQ(graph.preds_of(c)[0], a);
    EXPECT_EQ(graph.succs_of(c).size(), 1);
    EXPECT_EQ(graph.succs_of(c)[0], d);

    EXPECT_EQ(graph.preds_of(d).size(), 2);
    EXPECT_EQ(graph.preds_of(d)[0], b);
    EXPECT_EQ(graph.preds_of(d)[1], c);
    EXPECT_EQ(graph.succs_of(d).size(), 0);
}

TEST(GraphTest, Remove) {
    Graph<TestVertex> graph;
    auto *a = graph.emplace(0);
    auto *b = graph.emplace(1);
    auto *c = graph.emplace(2);
    auto *d = graph.emplace(1);
    EXPECT_EQ(graph.size(), 4);

    graph.remove(b);
    graph.remove(d);
    for (const auto &vertex : graph) {
        EXPECT_NE(vertex->num(), 1);
    }
    EXPECT_EQ(graph.size(), 2);
}

} // namespace
