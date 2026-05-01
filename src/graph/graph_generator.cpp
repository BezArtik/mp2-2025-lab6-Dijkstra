#include "graph/graph_generator.hpp"
#include "containers/vector.hpp"
#include "containers/stack.hpp"
#include "graph/graph.hpp"
#include <utility>
#include <random>
#include <algorithm>
#include <stdexcept>
#include <numeric>

namespace graph {

Graph GraphGenerator::generate(size_t vertices, double density,
    size_t min_weight, size_t max_weight) {
    if (vertices <= 0) {
        throw std::invalid_argument("Number of vertices must be positive");
    }
    if (density < 0.0 || density > 1.0) {
        throw std::invalid_argument("Density must be in [0.0, 1.0]");
    }

    Graph graph(vertices);

    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<size_t> weight_dist(min_weight, max_weight);
    std::uniform_real_distribution prob_dist(0.0, 1.0);

    containers::Vector<size_t> unvisited(vertices);
    std::iota(unvisited.begin(), unvisited.end(), 0);
    std::shuffle(unvisited.begin(), unvisited.end(), gen);

    containers::Vector<size_t> visited;
    visited.push_back(unvisited[0]);

    for (auto v : unvisited) {
        std::uniform_int_distribution<size_t> visitedDist(0, visited.size() - 1);
        auto u = visited[visitedDist(gen)]; 

        auto weight = weight_dist(gen);
        graph.add_edge(u, v, weight);
        graph.add_edge(v, u, weight);

        visited.push_back(v);
    }

    const auto max_edges = vertices * (vertices - 1) / 2;
    const auto target_edges = static_cast<size_t>(density * max_edges);
    const auto curr_edges = vertices - 1;

    containers::Vector<std::pair<size_t, size_t>> possible_edges;
    for (size_t i = 0; i < vertices; ++i) {
        for (size_t j = i + 1; j < vertices; ++j) {
            if (graph.get_edge(i, j) == graph::Graph::INF) {
                possible_edges.emplace_back(i, j);
            }
        }
    }

    std::shuffle(possible_edges.begin(), possible_edges.end(), gen);

    for (size_t i = 0; i < target_edges - curr_edges && i < possible_edges.size(); ++i) {
        auto [from, to] = possible_edges[i];
        auto weight = weight_dist(gen);
        graph.add_edge(from, to, weight);
        graph.add_edge(to, from, weight);
    }

    return graph;
}

bool GraphGenerator::is_connected(const Graph& graph) {
    auto V = graph.vertices();
    if (V == 0) return true;

    containers::Vector<bool> visited(V, false);
    containers::Stack<size_t> stack;

    stack.push(0);
    visited[0] = true;
    size_t visited_сount = 1;

    while (!stack.empty()) {
        auto vertex = stack.top();
        stack.pop();

        for (size_t i = 0; i < V; ++i) {
            if (!visited[i] && graph.get_edge(vertex, i) != Graph::INF) {
                visited[i] = true;
                stack.push(i);
                ++visited_сount;
            }
        }
    }

    return visited_сount == V;
}
    

}