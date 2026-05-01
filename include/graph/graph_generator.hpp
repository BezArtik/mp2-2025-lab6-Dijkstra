#pragma once
#include "containers/vector.hpp"
#include "graph/graph.hpp"

namespace graph {

struct GraphGenerator {
    static Graph generate(size_t vertices, double density,
        size_t minWeight = 1, size_t maxWeight = 100);

    static bool is_connected(const graph::Graph& graph);

};

}