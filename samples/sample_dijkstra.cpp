#include "containers/vector.hpp"
#include "graph/graph.hpp"
#include "graph/graph_generator.hpp"
#include "graph/dijkstra.hpp"
#include <iostream>
#include <exception>
#include <chrono>

template <typename HeapType>
void benchmark_dijkstra(const graph::Graph& graph) {
    const auto start = std::chrono::steady_clock::now();
    const auto dist = dijkstra<HeapType>(graph, 0);
    const auto finish = std::chrono::steady_clock::now();
    const std::chrono::duration<double> elapsed_seconds{ finish - start };
    std::cout << elapsed_seconds << std::endl;
}

int main() {
    try {
        std::cout << "Generate graph..." << std::endl;
        auto graph = graph::GraphGenerator::generate(5000, 1.0, 100, 1000);
        std::cout << "Complete." << std::endl;
        std::cout << "Dijkstra PQDHeap: ";
        benchmark_dijkstra<graph::PQDHeap>(graph);
        std::cout << "Dijkstra PQBinomialHeap: ";
        benchmark_dijkstra<graph::PQBinomialHeap>(graph);

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}