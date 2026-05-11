#include "graph/graph.hpp"
#include "graph/dijkstra.hpp"
#include <gtest/gtest.h>
#include <algorithm>

namespace tests {

class GraphTest : public ::testing::Test {
public:

    void verify_graph_consistency(const graph::Graph& graph) {
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

TEST_F(GraphTest, construction) {
    for (size_t n : {0, 1, 5, 10, 100}) {
        graph::Graph g(n);
        EXPECT_EQ(g.vertices(), n);
        EXPECT_EQ(g.edges(), 0);
    }
}

TEST_F(GraphTest, add_edges) {
    graph::Graph g(5);

    g.add_edge(0, 1, 5);
    EXPECT_TRUE(g.has_edge(0, 1));
    EXPECT_TRUE(g.has_edge(1, 0));
    EXPECT_EQ(g.edge(0, 1), 5);
    EXPECT_EQ(g.edge(1, 0), 5);
    EXPECT_EQ(g.edges(), 1);

    g.add_edge(2, 3, 10);
    EXPECT_TRUE(g.has_edge(2, 3));
    EXPECT_EQ(g.edge(2, 3), 10);
    EXPECT_EQ(g.edges(), 2);

    verify_graph_consistency(g);
}

TEST_F(GraphTest, update_edge_weight) {
    graph::Graph g(3);

    g.add_edge(0, 1, 5);
    EXPECT_EQ(g.edge(0, 1), 5);

    g.add_edge(0, 1, 10);
    EXPECT_EQ(g.edge(0, 1), 10);
    EXPECT_EQ(g.edges(), 1);

    g.add_edge(1, 2, 3);
    EXPECT_EQ(g.edges(), 2);

    g.add_edge(1, 0, 7);
    EXPECT_EQ(g.edge(0, 1), 7);
    EXPECT_EQ(g.edge(1, 0), 7);

    verify_graph_consistency(g);
}

TEST_F(GraphTest, self_loops) {
    graph::Graph g(5);

    g.add_edge(0, 0, 5);
    EXPECT_FALSE(g.has_edge(0, 0));
    EXPECT_EQ(g.edges(), 0);

    verify_graph_consistency(g);
}

TEST_F(GraphTest, vertex_validation) {
    graph::Graph g(5);

    ASSERT_ANY_THROW(g.add_edge(0, 5, 1));
    ASSERT_ANY_THROW(g.add_edge(5, 0, 1));
    ASSERT_ANY_THROW(g.add_edge(10, 10, 1));

    ASSERT_ANY_THROW(g.edge(0, 5));
    ASSERT_ANY_THROW(g.edge(5, 0));

    ASSERT_ANY_THROW(g.has_edge(0, 5));

    ASSERT_ANY_THROW(g.neighbors(5));
}

TEST_F(GraphTest, non_existent_edges) {
    graph::Graph g(5);

    EXPECT_FALSE(g.has_edge(0, 1));
    EXPECT_EQ(g.edge(0, 1), graph::Graph::INF);

    g.add_edge(0, 1, 5);

    EXPECT_FALSE(g.has_edge(0, 2));
    EXPECT_EQ(g.edge(0, 2), graph::Graph::INF);
}

TEST_F(GraphTest, neighbors) {
    graph::Graph g(5);

    g.add_edge(0, 1, 5);
    g.add_edge(0, 2, 10);
    g.add_edge(0, 3, 15);

    const auto& neighbors = g.neighbors(0);
    EXPECT_EQ(neighbors.size(), 3);

    for (const auto& [v, w] : neighbors) {
        if (v == 1) EXPECT_EQ(w, 5);
        if (v == 2) EXPECT_EQ(w, 10);
        if (v == 3) EXPECT_EQ(w, 15);
    }

    const auto& empty_neighbors = g.neighbors(4);
    EXPECT_TRUE(empty_neighbors.empty());
}

TEST_F(GraphTest, connectivity) {
    graph::Graph g(5);
    EXPECT_FALSE(g.is_connected());

    g.add_edge(0, 1, 1);
    g.add_edge(1, 2, 1);
    g.add_edge(2, 3, 1);
    g.add_edge(3, 4, 1);
    EXPECT_TRUE(g.is_connected());

    graph::Graph g2(5);
    g2.add_edge(0, 1, 1);
    g2.add_edge(1, 2, 1);
    g2.add_edge(3, 4, 1);
    EXPECT_FALSE(g2.is_connected());

    graph::Graph g3(1);
    EXPECT_TRUE(g3.is_connected());

    graph::Graph g4(2);
    EXPECT_FALSE(g4.is_connected());

    g4.add_edge(0, 1, 1);
    EXPECT_TRUE(g4.is_connected());
}

TEST_F(GraphTest, dijkstra_basic) {
    graph::Graph g(5);
    g.add_edge(0, 1, 4);
    g.add_edge(0, 2, 1);
    g.add_edge(2, 1, 2);
    g.add_edge(1, 3, 1);
    g.add_edge(2, 3, 5);
    g.add_edge(3, 4, 3);

    auto distances = graph::dijkstra<graph::DHeapDijkstra<3>>(g, 0);

    EXPECT_EQ(distances[0], 0);
    EXPECT_EQ(distances[1], 3);
    EXPECT_EQ(distances[2], 1);
    EXPECT_EQ(distances[3], 4);
    EXPECT_EQ(distances[4], 7);
}

TEST_F(GraphTest, dijkstra_unreachable) {
    graph::Graph g(4);
    g.add_edge(0, 1, 1);

    auto distances = graph::dijkstra<graph::BinomialDijkstra>(g, 0);

    EXPECT_EQ(distances[0], 0);
    EXPECT_EQ(distances[1], 1);
    EXPECT_EQ(distances[2], graph::Graph::INF);
    EXPECT_EQ(distances[3], graph::Graph::INF);
}

TEST_F(GraphTest, dijkstra_different_heaps) {
    graph::Graph g(6);
    g.add_edge(0, 1, 7);
    g.add_edge(0, 2, 9);
    g.add_edge(0, 5, 14);
    g.add_edge(1, 2, 10);
    g.add_edge(1, 3, 15);
    g.add_edge(2, 3, 11);
    g.add_edge(2, 5, 2);
    g.add_edge(3, 4, 6);
    g.add_edge(4, 5, 9);

    auto distances_dheap = graph::dijkstra<graph::DHeapDijkstra<3>>(g, 0);
    auto distances_binomial = graph::dijkstra<graph::BinomialDijkstra>(g, 0);

    EXPECT_TRUE(std::equal(
        distances_binomial.begin(),
        distances_binomial.end(),
        distances_dheap.begin()));

    EXPECT_EQ(distances_dheap[0], 0);
    EXPECT_EQ(distances_dheap[1], 7);
    EXPECT_EQ(distances_dheap[2], 9);
    EXPECT_EQ(distances_dheap[3], 20);
    EXPECT_EQ(distances_dheap[4], 20);
    EXPECT_EQ(distances_dheap[5], 11);
}

}
