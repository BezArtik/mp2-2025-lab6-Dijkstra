#pragma once
#include "containers/vector.hpp"

namespace graph {

class Graph {
public:
	static constexpr size_t INF = std::numeric_limits<size_t>::max();
	Graph(size_t vertices);
	void add_edge(size_t from, size_t to, size_t weight);
	size_t get_edge(size_t from, size_t to) const;
	size_t vertices() const noexcept;

private:
	containers::Vector<size_t> matrix_{};
	size_t vertices_{};

	size_t index(size_t row, size_t col) const noexcept;
};

}