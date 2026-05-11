#include "graph/graph_generator.hpp"
#include "graph/graph.hpp"
#include "containers/vector.hpp"
#include <random>
#include <numeric>
#include <stdexcept>
#include <algorithm>
#include <utility>

namespace graph {

Graph generate(size_t vertices, double density, size_t min_weight, size_t max_weight) {
    if (vertices <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    if (density < 0.0 || density > 1.0) {
        throw std::invalid_argument("Density must be in [0.0, 1.0]");
    }

    Graph graph(vertices);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<size_t> dist_weight(min_weight, max_weight);
    std::uniform_real_distribution prob_dist(0.0, 1.0);

    containers::Vector<size_t> vertices_list(vertices);
    std::iota(vertices_list.begin(), vertices_list.end(), 0);
    std::shuffle(vertices_list.begin(), vertices_list.end(), gen);

    for (size_t i = 1; i < vertices; ++i) {
        auto u = vertices_list[i];
        auto v = vertices_list[std::uniform_int_distribution<size_t>(0, i - 1)(gen)];

        auto weight = dist_weight(gen);
        graph.add_edge(u, v, weight);
    }

    const auto max_edges = vertices * (vertices - 1) / 2;
    const auto target_edges = static_cast<size_t>(density * max_edges);
    const auto curr_edges = vertices - 1;

    containers::Vector<std::pair<size_t, size_t>> possible_edges;
    for (size_t i = 0; i < vertices; ++i) {
        for (size_t j = i + 1; j < vertices; ++j) {
            if (!graph.has_edge(i, j)) {
                possible_edges.emplace_back(i, j);
            }
        }
    }

    std::shuffle(possible_edges.begin(), possible_edges.end(), gen);

    auto edges_to_add = std::min(target_edges - curr_edges,
        possible_edges.size());
    for (size_t i = 0; i < edges_to_add; ++i) {
        auto [from, to] = possible_edges[i];
        auto weight = dist_weight(gen);
        graph.add_edge(from, to, weight);
    }

    return graph;
}

}