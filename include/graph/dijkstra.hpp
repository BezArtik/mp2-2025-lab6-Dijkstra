#pragma once
#include "graph/graph.hpp"
#include "containers/vector.hpp"
#include "heaps/d_heap.hpp"
#include "heaps/binomial_heap.hpp"
#include "containers/priority_queue.hpp"

namespace graph {

template <typename PriorityQueue>
auto dijkstra(const graph::Graph& graph, size_t start) {
    const auto n = graph.vertices();
    containers::Vector<size_t> distances(n, Graph::INF);
    distances[start] = 0;

    PriorityQueue pq;
    pq.push({ 0, start });
    while (!pq.empty()) {
        auto [dist, u] = pq.top();
        pq.pop();
        if (dist > distances[u]) continue;
        for (const auto& [v, weight] : graph.neighbors(u)) {
            auto new_dist = dist + weight;
            if (new_dist < distances[v]) {
                distances[v] = new_dist;
                pq.push({ new_dist, v });
            }
        }
    }
    return distances;
}

void print_distances(const containers::Vector<size_t>& distances, size_t start);

template <int32_t d = 3>
using DHeapDijkstra = containers::PriorityQueue<
    std::pair<size_t, size_t>,
    heaps::DHeap<std::pair<size_t, size_t>, d, std::greater<std::pair<size_t, size_t>>>,
    std::greater<std::pair<size_t, size_t>>
>;

using BinomialDijkstra = containers::PriorityQueue<
    std::pair<size_t, size_t>,
    heaps::BinomialHeap<std::pair<size_t, size_t>, std::greater<std::pair<size_t, size_t>>>,
    std::greater<std::pair<size_t, size_t>>
>;

}