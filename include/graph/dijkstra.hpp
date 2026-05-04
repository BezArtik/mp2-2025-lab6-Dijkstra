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

template <typename PriorityQueue, typename WeightType = size_t>
auto dijkstra(const graph::Graph<WeightType>& graph, size_t start) {
	const auto n = graph.vertices();
	containers::Vector<WeightType> distances(n, Graph<WeightType>::INF);
	distances[start] = WeightType{ 0 };

	PriorityQueue pq;
	pq.push({ WeightType{ 0 }, start });
	while (!pq.empty()) {
		auto [dist, u] = pq.top();
		pq.pop();
		if (dist > distances[u]) continue;

		for (const auto& [v, weight] : graph.get_neighbors(u)) {
			auto new_dist = dist + weight;
			if (new_dist < distances[v]) {
				distances[v] = new_dist;
				pq.push({ new_dist, v });
			}
		}
	}
	return distances;
}

template <typename WeightType>
void print_distances(const containers::Vector<WeightType>& distances, size_t start) {
	std::cout << "Distances from vertex " << start << ":\n";
	for (size_t i = 0; i < distances.size(); ++i) {
		if (distances[i] == Graph<WeightType>::INF) {
			std::cout << i << ": unreachable\n";
		} else {
			std::cout << i << ": " << distances[i] << '\n';
		}
	}
}

template <typename WeightType, int32_t d = 3>
using DHeapDijkstra = containers::PriorityQueue<
	std::pair<WeightType, size_t>,
	heaps::DHeap<std::pair<WeightType, size_t>, d, std::greater<std::pair<WeightType, size_t>>>,
	std::greater<std::pair<WeightType, size_t>>
>;

template <typename WeightType>
using BinomialDijkstra = containers::PriorityQueue<
	std::pair<WeightType, size_t>,
	heaps::BinomialHeap<std::pair<WeightType, size_t>, std::greater<std::pair<WeightType, size_t>>>,
	std::greater<std::pair<WeightType, size_t>>
>;

}