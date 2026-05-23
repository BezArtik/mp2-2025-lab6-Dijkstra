#pragma once
#include "containers/vector.hpp"
#include <limits>

namespace graph {

class Graph {    
public:
    static constexpr size_t INF = std::numeric_limits<size_t>::max() / 2;
    static constexpr double DENSE_THRESHOLD = 0.6;
    static constexpr double SPARSE_THRESHOLD = 0.2;

    Graph(size_t vertices);

    void add_edge(size_t from, size_t to, size_t weight);

    const containers::Vector<std::pair<size_t, size_t>>& neighbors(size_t vertex) const;

    size_t edge(size_t from, size_t to) const;

    bool has_edge(size_t from, size_t to) const;

    bool is_connected() const;

    size_t vertices() const noexcept;
    size_t edges() const noexcept;

private:

    void validate(size_t v) const;
    void validate(size_t from, size_t to) const;

    double density() const noexcept;

    bool add_edge_list(size_t from, size_t to, size_t weight);

    size_t edge_list(size_t from, size_t to) const noexcept;

    bool should_enable_matrix() const noexcept;

    bool should_disable_matrix() const noexcept;

    void build_matrix();

    void drop_matrix() noexcept;

    size_t vertices_;
    size_t edges_;
    bool use_matrix_;

    containers::Vector<containers::Vector<std::pair<size_t, size_t>>> adj_list_;
    containers::Vector<size_t> matrix_;

};

} 