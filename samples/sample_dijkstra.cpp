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
#include <limits>

namespace sample {

struct BenchmarkResult {
    std::string name_;
    double elapsed_seconds_;
};

template <typename PQType>
BenchmarkResult benchmark_dijkstra(
    const graph::Graph& graph,
    const std::string& name,
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

    return {name, total_time / iterations};
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

void compare_heaps(const graph::Graph& graph) {
    print_benchmark_header();

    using DHeap2 = graph::DHeapDijkstra<2>;
    using DHeap3 = graph::DHeapDijkstra<3>;
    using Binomial = graph::BinomialDijkstra;

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

void clear_input_buffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::optional<GraphConfig> get_graph_config() noexcept {
    GraphConfig config;

    std::cout << "\n=== Graph Configuration ===\n";

    std::cout << "Number of vertices (0 to exit): ";
    if (!(std::cin >> config.vertices_) || config.vertices_ == 0) {
        clear_input_buffer();
        return std::nullopt;
    }

    std::cout << "Density [0.0, 1.0]: ";
    if (!(std::cin >> config.density_) || config.density_ <= 0.0 || config.density_ > 1.0) {
        std::cerr << "Invalid density. Using default: 0.3\n";
        config.density_ = 0.3;
        clear_input_buffer();
    }

    std::cout << "Min and max weight: ";
    if (!(std::cin >> config.min_weight_ >> config.max_weight_) ||
        config.min_weight_ > config.max_weight_) {
        std::cerr << "Invalid weights. Using default: 1 100\n";
        config.min_weight_ = 1;
        config.max_weight_ = 100;
        clear_input_buffer();
    }

    return config;
}

auto input_graph_manual() {
    size_t vertices = 0;
    std::cout << "Number of vertices: ";
    if (!(std::cin >> vertices)) {
        std::cerr << "Invalid vertices. Using default 2\n";
        vertices = 2;
        clear_input_buffer();
    }

    graph::Graph graph(vertices);

    size_t edges_count;
    std::cout << "Number of edges: ";
    if (!(std::cin >> edges_count)) {
        std::cerr << "Invalid number of edges. Using default 1\n";
        edges_count = 1;
        clear_input_buffer();
    }

    std::cout << "Enter " << edges_count << " edges (from to weight):\n";
    for (size_t i = 0; i < edges_count; ++i) {
        size_t from = 0, to = 0, weight = 0;
        if (!(std::cin >> from >> to >> weight)) {
            std::cerr << "Invalid input. Using default data\n";
            from = 0; to = 1; weight = 1;
            clear_input_buffer();
        }

        try {
            graph.add_edge(from, to, weight);
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << '\n';
        }
    }

    return graph;
}

void run_dijkstra_interactive(const graph::Graph& graph) {
    std::cout << "\n=== Dijkstra ===\n";

    size_t start = 0;
    std::cout << "Start vertex: ";

    if (!(std::cin >> start) || start >= graph.vertices()) {
        std::cerr << "Invalid vertex. Using default 0\n";
        clear_input_buffer();
    }

    auto distances = graph::dijkstra<graph::DHeapDijkstra<3>>(graph, start);
    graph::print_distances(distances, start);
}


void run_interactive() {
    while (true) {
        std::cout << "\n=== Main Menu ===\n";
        std::cout << "1: Generate random graph + benchmark\n"
                  << "2: Input graph manually + Dijkstra\n"
                  << "0: Exit\n"
                  << "Choice: ";

        int choice = 0;
        std::cin >> choice;
        clear_input_buffer();

        if (choice == 0) {
            std::cout << "Exiting...\n";
            break;
        }

        if (choice == 1) {
            auto config = get_graph_config();
            if (!config) continue;

            std::cout << "\nGenerating graph...\n";
            auto graph = graph::generate(
                config->vertices_, config->density_,
                config->min_weight_, config->max_weight_);
            std::cout << "Complete.\n\n";
            compare_heaps(graph);
            std::cout << std::string(40, '-') << '\n';
        } else if (choice == 2) {
            auto graph = input_graph_manual();

            if (!graph.is_connected()) {
                std::cout << "Graph is not connected!\n";
                continue;
            }

            run_dijkstra_interactive(graph);
            std::cout << std::string(40, '-') << '\n';
        }
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