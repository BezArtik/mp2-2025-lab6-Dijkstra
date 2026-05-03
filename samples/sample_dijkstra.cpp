#include "graph/graph.hpp"
#include "graph/graph_generator.hpp"
#include "graph/dijkstra.hpp"
#include <iostream>
#include <exception>
#include <chrono>
#include <string>
#include <iomanip>
#include <optional>
#include <initializer_list>

namespace sample {

struct BenchmarkResult {
    std::string name_;
    double elapsed_seconds_;
};

template <typename PQType>
BenchmarkResult benchmark_dijkstra(
    const graph::Graph<size_t>& graph,
    const std::string& name_,
    size_t iterations) {
    graph::dijkstra<PQType>(graph, 0);

    double total_time = 0.0;
    for (size_t i = 0; i < iterations; ++i) {
        const auto start = std::chrono::steady_clock::now();
        const auto dist = graph::dijkstra<PQType>(graph, 0);
        const auto finish = std::chrono::steady_clock::now();

        std::chrono::duration<double> elapsed = finish - start;
        total_time += elapsed.count();
    }

    return {name_, total_time / iterations};
}

void print_benchmark_header() noexcept {
    std::cout 
        << std::left 
        << std::setw(25) << "Heap"
        << std::setw(12) << "Time (ms)"
        << '\n';
    std::cout << std::string(40, '-') << '\n';
}

void print_benchmark_result(const BenchmarkResult& result) noexcept {
    std::cout 
        << std::left
        << std::setw(25) << result.name_
        << std::fixed << std::setprecision(3)
        << std::setw(12) << result.elapsed_seconds_ * 1000 
        << '\n';
}

void compare_heaps(const graph::Graph<size_t>& graph) {
    print_benchmark_header();

    using DHeap2 = graph::DHeapDijkstra<size_t, 2>;
    using DHeap3 = graph::DHeapDijkstra<size_t>;
    using Binomial = graph::BinomialDijkstra<size_t>;

    auto results = {
        benchmark_dijkstra<DHeap2>(graph, "2-Heap", 10),
        benchmark_dijkstra<DHeap3>(graph, "3-Heap", 10),
        benchmark_dijkstra<Binomial>(graph, "Binomial Heap", 10),
    };

    for (const auto& res : results) {
        print_benchmark_result(res);
    }
}

struct GraphConfig {
    size_t vertices_;
    double density_;
    size_t min_weight_;
    size_t max_weight_;
};

std::optional<GraphConfig> get_graph_config() noexcept {
    GraphConfig config;

    std::cout << "\n=== Graph Configuration ===\n";

    std::cout << "Number of vertices (0 to exit): ";
    if (!(std::cin >> config.vertices_) || config.vertices_ == 0) {
        return std::nullopt;
    }

    std::cout << "Density [0.0, 1.0]: ";
    if (!(std::cin >> config.density_) || config.density_ < 0.0 || config.density_ > 1.0) {
        std::cerr << "Invalid density. Using default: 0.3\n";
        config.density_ = 0.3;
    }

    std::cout << "Min and max weight: ";
    if (!(std::cin >> config.min_weight_ >> config.max_weight_) ||
        config.min_weight_ > config.max_weight_) {
        std::cerr << "Invalid weights. Using default: 1 100\n";
        config.min_weight_ = 1;
        config.max_weight_ = 100;
    }

    return config;
}

void run_interactive() {
    while (true) {
        auto config = get_graph_config();
        if (!config) {
            std::cout << "Exiting...\n";
            break;
        }

        std::cout << "\nGenerating graph...\n";

        auto graph = graph::generate(
            config->vertices_, config->density_,
            config->min_weight_, config->max_weight_);

        std::cout << "Complete." << "\n\n";

        compare_heaps(graph);
        std::cout << std::string(40, '-') << '\n';
    }
}

} 

int main() {
    try {
        sample::run_interactive();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}