#pragma once
#include "containers/vector.hpp"
#include "containers/stack.hpp"
#include <limits>

namespace graph {

template <typename WeightType = size_t>
class Graph {
public:
    static constexpr WeightType INF = std::numeric_limits<WeightType>::max() / 2;
    static constexpr double DENSITY = 0.5; 

    Graph(size_t vertices)
        : vertices_(vertices)
        , adj_list_(vertices)
        , use_matrix_(false)
        , edges_(0) {}

    void add_edge(size_t from, size_t to, WeightType weight) {
        validate(from, to);
        if (from == to) return;

        if (!use_matrix_) {
            add_edge_list(from, to, weight);
        } else {
            add_edge_matrix(from, to, weight);
        }

        ++edges_;

        if (!use_matrix_ && should_use_matrix()) {
            switch_to_matrix();
        }
    }

    WeightType get_edge(size_t from, size_t to) const {
        validate(from, to);

        if (!use_matrix_) {
            return get_edge_list(from, to);
        } else {
            return get_edge_matrix(from, to);
        }
    }

    bool has_edge(size_t from, size_t to) const {
        return get_edge(from, to) != INF;
    }

    const auto& get_neighbors(size_t vertex) const {
        validate(vertex);

        if (!use_matrix_) {
            return adj_list_[vertex];
        } else {
            if (cached_vertex_ != vertex) {
                cache_neighbors(vertex);
            }
            return neighbor_cache_;
        }
    }

    static bool is_connected(const Graph<WeightType>& graph) {
        auto V = graph.vertices();
        if (V == 0) return true;

        containers::Vector<bool> visited(V, false);
        containers::Stack<size_t> stack;

        stack.push(0);
        visited[0] = true;
        size_t visited_сount = 1;

        while (!stack.empty()) {
            auto vertex = stack.top();
            stack.pop();

            for (size_t i = 0; i < V; ++i) {
                if (!visited[i] && graph.get_edge(vertex, i) != INF) {
                    visited[i] = true;
                    stack.push(i);
                    ++visited_сount;
                }
            }
        }

        return visited_сount == V;
    }

    size_t vertices() const noexcept { return vertices_; }
    size_t edges() const noexcept { return edges_; }
    bool is_dense() const noexcept { return use_matrix_; }

private:

    void validate(size_t v) const {
        if (v >= vertices_) throw std::out_of_range("Vertex index out of range");
    }

    void validate(size_t from, size_t to) const {
        validate(from);
        validate(to);
    }

    void add_edge_list(size_t from, size_t to, WeightType weight) {
        for (auto& [v, w] : adj_list_[from]) {
            if (v == to) {
                w = weight;
                for (auto& [v2, w2] : adj_list_[to]) {
                    if (v2 == from) { w2 = weight; return; }
                }
                return;
            }
        }
        adj_list_[from].push_back({ to, weight });
        adj_list_[to].push_back({ from, weight });
    }

    void add_edge_matrix(size_t from, size_t to, WeightType weight) noexcept {
        matrix_[from * vertices_ + to] = weight;
        matrix_[to * vertices_ + from] = weight;
    }

    WeightType get_edge_list(size_t from, size_t to) const noexcept {
        for (const auto& [v, w] : adj_list_[from]) {
            if (v == to) return w;
        }
        return INF;
    }

    WeightType get_edge_matrix(size_t from, size_t to) const noexcept {
        return matrix_[from * vertices_ + to];
    }

    bool should_use_matrix() const noexcept {
        if (vertices_ < 2) return false;
        auto max_edges = vertices_ * (vertices_ - 1) / 2;
        return static_cast<double>(edges_) / max_edges >= DENSITY;
    }

    void switch_to_matrix() {
        matrix_.resize(vertices_ * vertices_, INF);
        for (size_t i = 0; i < vertices_; ++i) {
            matrix_[i * vertices_ + i] = 0;
        }

        for (size_t from = 0; from < vertices_; ++from) {
            for (const auto& [to, weight] : adj_list_[from]) {
                matrix_[from * vertices_ + to] = weight;
            }
        }

        adj_list_.clear();
        use_matrix_ = true;
    }

    void cache_neighbors(size_t vertex) const {
        neighbor_cache_.clear();
        auto base = vertex * vertices_;
        for (size_t i = 0; i < vertices_; ++i) {
            if (i != vertex) {
                auto w = matrix_[base + i];
                if (w != INF) {
                    neighbor_cache_.push_back({ i, w });
                }
            }
        }
        cached_vertex_ = vertex;
    }

    size_t vertices_;
    size_t edges_;
    bool use_matrix_;

    using vertices_list = containers::Vector<std::pair<size_t, WeightType>>;
    containers::Vector<vertices_list> adj_list_;
    containers::Vector<WeightType> matrix_;

    mutable vertices_list neighbor_cache_;
    mutable size_t cached_vertex_ = std::numeric_limits<size_t>::max();
};

}