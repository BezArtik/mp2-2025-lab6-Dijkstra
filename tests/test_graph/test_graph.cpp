#include "graph/graph.hpp"
#include "graph/dijkstra.hpp"
#include <gtest/gtest.h>
#include <algorithm>
#include <cstdint>

namespace tests {

template <typename WeightType>
class GraphTest : public ::testing::Test {
protected:
    static WeightType W(int32_t value) { return static_cast<WeightType>(value); }

    void verify_graph_consistency(const graph::Graph<WeightType>& graph) {
        size_t edges_count = 0;
        for (size_t i = 0; i < graph.vertices(); ++i) {
            for (size_t j = i + 1; j < graph.vertices(); ++j) {
                bool has_edge_ij = graph.has_edge(i, j);
                bool has_edge_ji = graph.has_edge(j, i);
                EXPECT_EQ(has_edge_ij, has_edge_ji);

                if (has_edge_ij && has_edge_ji) 
                    EXPECT_EQ(graph.edge(i, j), graph.edge(j, i));
                if (has_edge_ij) ++edges_count;
            }
        }
        EXPECT_EQ(graph.edges(), edges_count);
    }
};

using WeightTypes = ::testing::Types<size_t, int32_t, double>;
TYPED_TEST_SUITE(GraphTest, WeightTypes);

TYPED_TEST(GraphTest, construction) {
    for (size_t n : {0, 1, 5, 10, 100}) {
        graph::Graph<TypeParam> g(n);
        EXPECT_EQ(g.vertices(), n);
        EXPECT_EQ(g.edges(), 0);
    }
}

TYPED_TEST(GraphTest, add_edges) {
    graph::Graph<TypeParam> g(5);

    g.add_edge(0, 1, this->W(5));
    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_TRUE(g.has_edge(1, 0));
    EXPECT_EQ(g.edge(0, 1), this->W(5));
    EXPECT_EQ(g.edge(1, 0), this->W(5));
    EXPECT_EQ(g.edges(), 1);

    g.add_edge(2, 3, this->W(10));
    EXPECT_TRUE(g.has_edge(2, 3));
    EXPECT_EQ(g.edge(2, 3), this->W(10));
    EXPECT_EQ(g.edges(), 2);

    this->verify_graph_consistency(g);
}

TYPED_TEST(GraphTest, update_edge_weight) {
    graph::Graph<TypeParam> g(3);

    g.add_edge(0, 1, this->W(5));
    EXPECT_EQ(g.edge(0, 1), this->W(5));

    g.add_edge(0, 1, this->W(10));
    EXPECT_EQ(g.edge(0, 1), this->W(10));
    EXPECT_EQ(g.edges(), 1);

    g.add_edge(1, 2, this->W(3));
    EXPECT_EQ(g.edges(), 2);

    g.add_edge(1, 0, this->W(7));
    EXPECT_EQ(g.edge(0, 1), this->W(7));
    EXPECT_EQ(g.edge(1, 0), this->W(7));

    this->verify_graph_consistency(g);
}

TYPED_TEST(GraphTest, self_loops) {
    graph::Graph<TypeParam> g(5);

    g.add_edge(0, 0, this->W(5));
    EXPECT_FALSE(g.has_edge(0, 0));
    EXPECT_EQ(g.edges(), 0);

    this->verify_graph_consistency(g);
}

TYPED_TEST(GraphTest, vertex_validation) {
    graph::Graph<TypeParam> g(5);

    ASSERT_ANY_THROW(g.add_edge(0, 5, this->W(1)));
    ASSERT_ANY_THROW(g.add_edge(5, 0, this->W(1)));
    ASSERT_ANY_THROW(g.add_edge(10, 10, this->W(1)));

    ASSERT_ANY_THROW(g.edge(0, 5));
    ASSERT_ANY_THROW(g.edge(5, 0));

    ASSERT_ANY_THROW(g.has_edge(0, 5));

    ASSERT_ANY_THROW(g.neighbors(5));
}

TYPED_TEST(GraphTest, non_existent_edges) {
    graph::Graph<TypeParam> g(5);

    EXPECT_FALSE(g.has_edge(0, 1));
    EXPECT_EQ(g.edge(0, 1), graph::Graph<TypeParam>::INF);

    g.add_edge(0, 1, this->W(5));

    EXPECT_FALSE(g.has_edge(0, 2));
    EXPECT_EQ(g.edge(0, 2), graph::Graph<TypeParam>::INF);
}

TYPED_TEST(GraphTest, neighbors) {
    graph::Graph<TypeParam> g(5);

    g.add_edge(0, 1, this->W(5));
    g.add_edge(0, 2, this->W(10));
    g.add_edge(0, 3, this->W(15));

    const auto& neighbors = g.neighbors(0);
    EXPECT_EQ(neighbors.size(), 3);

    for (const auto& [v, w] : neighbors) {
        if (v == 1) EXPECT_EQ(w, this->W(5));
        if (v == 2) EXPECT_EQ(w, this->W(10));
        if (v == 3) EXPECT_EQ(w, this->W(15));
    }

    const auto& empty_neighbors = g.neighbors(4);
    EXPECT_TRUE(empty_neighbors.empty());
}

TYPED_TEST(GraphTest, connectivity) {
    graph::Graph<TypeParam> g(5);
    EXPECT_FALSE(g.is_connected());

    g.add_edge(0, 1, this->W(1));
    g.add_edge(1, 2, this->W(1));
    g.add_edge(2, 3, this->W(1));
    g.add_edge(3, 4, this->W(1));
    EXPECT_TRUE(g.is_connected());

    graph::Graph<TypeParam> g2(5);
    g2.add_edge(0, 1, this->W(1));
    g2.add_edge(1, 2, this->W(1));
    g2.add_edge(3, 4, this->W(1));
    EXPECT_FALSE(g2.is_connected());

    graph::Graph<TypeParam> g3(1);
    EXPECT_TRUE(g3.is_connected());

    graph::Graph<TypeParam> g4(2);
    EXPECT_FALSE(g4.is_connected());

    g4.add_edge(0, 1, this->W(1));
    EXPECT_TRUE(g4.is_connected());
}

TYPED_TEST(GraphTest, dijkstra_basic) {
    graph::Graph<TypeParam> g(5);
    g.add_edge(0, 1, this->W(4));
    g.add_edge(0, 2, this->W(1));
    g.add_edge(2, 1, this->W(2));
    g.add_edge(1, 3, this->W(1));
    g.add_edge(2, 3, this->W(5));
    g.add_edge(3, 4, this->W(3));

    auto distances = graph::dijkstra<graph::DHeapDijkstra<TypeParam>>(g, 0);

    EXPECT_EQ(distances[0], this->W(0));
    EXPECT_EQ(distances[1], this->W(3));
    EXPECT_EQ(distances[2], this->W(1));
    EXPECT_EQ(distances[3], this->W(4));
    EXPECT_EQ(distances[4], this->W(7));
}

TYPED_TEST(GraphTest, dijkstra_unreachable) {
    graph::Graph<TypeParam> g(4);
    g.add_edge(0, 1, this->W(1));

    auto distances = graph::dijkstra<graph::BinomialDijkstra<TypeParam>>(g, 0);

    EXPECT_EQ(distances[0], this->W(0));
    EXPECT_EQ(distances[1], this->W(1));
    EXPECT_EQ(distances[2], graph::Graph<TypeParam>::INF);
    EXPECT_EQ(distances[3], graph::Graph<TypeParam>::INF);
}

TYPED_TEST(GraphTest, dijkstra_different_heaps) {
    graph::Graph<TypeParam> g(6);
    g.add_edge(0, 1, this->W(7));
    g.add_edge(0, 2, this->W(9));
    g.add_edge(0, 5, this->W(14));
    g.add_edge(1, 2, this->W(10));
    g.add_edge(1, 3, this->W(15));
    g.add_edge(2, 3, this->W(11));
    g.add_edge(2, 5, this->W(2));
    g.add_edge(3, 4, this->W(6));
    g.add_edge(4, 5, this->W(9));

    auto distances_dheap = graph::dijkstra<graph::DHeapDijkstra<TypeParam>>(g, 0);
    auto distances_binomial = graph::dijkstra<graph::BinomialDijkstra<TypeParam>>(g, 0);

    EXPECT_TRUE(std::equal(
        distances_binomial.begin(),
        distances_binomial.end(),
        distances_dheap.begin()));

    EXPECT_EQ(distances_dheap[0], this->W(0));
    EXPECT_EQ(distances_dheap[1], this->W(7));
    EXPECT_EQ(distances_dheap[2], this->W(9));
    EXPECT_EQ(distances_dheap[3], this->W(20));
    EXPECT_EQ(distances_dheap[4], this->W(20));
    EXPECT_EQ(distances_dheap[5], this->W(11));
}

}
