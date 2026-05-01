#pragma once
#include "containers/vector.hpp"
#include "containers/priority_queue.hpp"
#include "heaps/d_heap.hpp"
#include "heaps/binomial_heap.hpp"
#include "graph/graph.hpp"
#include "graph/graph_generator.hpp"
#include <iostream>
#include <exception>
#include <utility>
#include <limits>
#include <iomanip>

namespace graph {

using PQElement = std::pair<size_t, size_t>;
using Compare = std::greater<PQElement>;
using PQDHeap = heaps::DHeap<PQElement, 3, Compare>;
using PQBinomialHeap = heaps::BinomialHeap<PQElement, Compare>;

template <typename HeapType>
auto dijkstra(const graph::Graph& graph, size_t start) {
	const auto n = graph.vertices();
	containers::Vector<size_t> distances(n, Graph::INF);
	distances[start] = 0;
	containers::PriorityQueue<PQElement, HeapType, Compare> pq;
	pq.push({ 0, start });
	while (!pq.empty()) {
		auto [dist, u] = pq.top();
		pq.pop();
		if (dist > distances[u]) continue;
		for (size_t v = 0; v < n; ++v) {
			const auto weight = graph.get_edge(u, v);
			if (weight != Graph::INF) {
				auto new_dist = dist + weight;
				if (new_dist < distances[v]) {
					distances[v] = new_dist;
					pq.push({ new_dist, v });
				}
			}
		}
	}
	return distances;
}

}