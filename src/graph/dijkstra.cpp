#include "graph/dijkstra.hpp"
#include "graph/graph.hpp"
#include "containers/vector.hpp"
#include <iostream>

namespace graph {

void print_distances(const containers::Vector<size_t>& distances, size_t start) {
    std::cout << "Distances from vertex " << start << ":\n";
    for (size_t i = 0; i < distances.size(); ++i) {
        if (distances[i] == Graph::INF) {
            std::cout << i << ": unreachable\n";
        } else {
            std::cout << i << ": " << distances[i] << '\n';
        }
    }
}

}