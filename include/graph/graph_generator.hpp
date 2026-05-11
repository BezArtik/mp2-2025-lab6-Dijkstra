#pragma once 
#include "graph/graph.hpp"

namespace graph {

Graph generate(size_t vertices, double density, size_t min_weight, size_t max_weight);

}