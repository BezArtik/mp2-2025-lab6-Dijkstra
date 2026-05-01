#include "graph/graph.hpp"
#include <stdexcept>

namespace graph {

Graph::Graph(size_t vertices) : vertices_(vertices), matrix_(vertices * vertices, INF) {
	for (size_t i = 0; i < vertices; ++i) {
		matrix_[index(i, i)] = 0;
	}
}

void Graph::add_edge(size_t from, size_t to, size_t weight) {
	if (from >= vertices_ || to >= vertices_) {
		throw std::out_of_range("Vertex index out of range");
	}
	matrix_[index(from, to)] = weight;
}

size_t Graph::get_edge(size_t from, size_t to) const {
	if (from >= vertices_ || to >= vertices_) {
		throw std::out_of_range("Vertex index out of range");
	}
	return matrix_[index(from, to)];
}

size_t Graph::vertices() const noexcept { return vertices_; }

size_t Graph::index(size_t row, size_t col) const noexcept {
	return row * vertices_ + col;
}

}