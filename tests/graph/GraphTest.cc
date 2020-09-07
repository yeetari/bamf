#include <bamf/graph/Graph.hh>

#include "TestData.hh"

#include <gtest/gtest.h>

namespace bamf {

namespace {

TEST(GraphTest, Disconnect) {
    Graph<TestVertex> graph;
    auto *a = graph.emplace(0);
    auto *b = graph.emplace(1);
    auto *c = graph.emplace(2);
    auto *d = graph.emplace(3);
    graph.connect<TestEdge>(a, b);
    graph.connect<TestEdge>(a, c);
    graph.connect<TestEdge>(b, d);
    graph.connect<TestEdge>(c, d);
    EXPECT_EQ(graph.preds(d).size(), 2);
    EXPECT_EQ(graph.preds(d)[0], b);
    EXPECT_EQ(graph.preds(d)[1], c);

    graph.disconnect(c, d);
    EXPECT_EQ(graph.preds(d).size(), 1);
    EXPECT_EQ(graph.preds(d)[0], b);

    graph.disconnect(b, d);
    EXPECT_EQ(graph.preds(d).size(), 0);
}

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

    EXPECT_EQ(graph.preds(a).size(), 0);
    EXPECT_EQ(graph.succs(a).size(), 2);
    EXPECT_EQ(graph.succs(a)[0], b);
    EXPECT_EQ(graph.succs(a)[1], c);

    EXPECT_EQ(graph.preds(b).size(), 1);
    EXPECT_EQ(graph.preds(b)[0], a);
    EXPECT_EQ(graph.succs(b).size(), 1);
    EXPECT_EQ(graph.succs(b)[0], d);

    EXPECT_EQ(graph.preds(c).size(), 1);
    EXPECT_EQ(graph.preds(c)[0], a);
    EXPECT_EQ(graph.succs(c).size(), 1);
    EXPECT_EQ(graph.succs(c)[0], d);

    EXPECT_EQ(graph.preds(d).size(), 2);
    EXPECT_EQ(graph.preds(d)[0], b);
    EXPECT_EQ(graph.preds(d)[1], c);
    EXPECT_EQ(graph.succs(d).size(), 0);
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

} // namespace bamf
