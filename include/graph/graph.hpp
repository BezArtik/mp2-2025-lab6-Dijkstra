#pragma once
#include "containers/vector.hpp"
#include "containers/stack.hpp"
#include <limits>

namespace graph {

template <typename WeightType = size_t>
class Graph {
public:
    static constexpr WeightType INF = std::numeric_limits<WeightType>::max() / 2;
    static constexpr double DENSE_THRESHOLD = 0.3;
    static constexpr double SPARSE_THRESHOLD = 0.1;

    Graph(size_t vertices)
        : vertices_(vertices)
        , edges_(0)
        , use_matrix_(false)         
        , adj_list_(vertices) {}

    void add_edge(size_t from, size_t to, WeightType weight) {
        validate(from, to);
        if (from == to) return;

        add_edge_list(from, to, weight);

        if (use_matrix_) {
            matrix_[from * vertices_ + to] = weight;
            matrix_[to * vertices_ + from] = weight;
        }

        ++edges_;

        if (!use_matrix_ && should_enable_matrix()) {
            build_matrix();
        } else if (use_matrix_ && should_disable_matrix()) {
            drop_matrix();
        }
    }

    const auto& get_neighbors(size_t vertex) const {
        validate(vertex);
        return adj_list_[vertex];
    }

    WeightType get_edge(size_t from, size_t to) const {
        validate(from, to);

        if (use_matrix_) {
            return matrix_[from * vertices_ + to]; 
        } else {
            return get_edge_list(from, to);
        }
    }

    bool has_edge(size_t from, size_t to) const {
        return get_edge(from, to) != INF;
    }

    bool is_connected() const {
        if (vertices_ == 0) return true;

        containers::Vector<bool> visited(vertices_, false);
        containers::Stack<size_t> stack;

        stack.push(0);
        visited[0] = true;
        size_t visited_count = 1;

        while (!stack.empty()) {
            auto vertex = stack.top();
            stack.pop();

            for (const auto& [neighbor, weight] : get_neighbors(vertex)) {
                if (!visited[neighbor]) {
                    visited[neighbor] = true;
                    stack.push(neighbor);
                    ++visited_count;
                }
            }
        }

        return visited_count == vertices_;
    }

    size_t vertices() const noexcept { return vertices_; }
    size_t edges() const noexcept { return edges_; }

private:

    void validate(size_t v) const {
        if (v >= vertices_) throw std::out_of_range("Vertex index out of range");
    }

    void validate(size_t from, size_t to) const {
        validate(from);
        validate(to);
    }

    double density() const noexcept {
        if (vertices_ < 2) return 0.0;
        return static_cast<double>(edges_) / (vertices_ * (vertices_ - 1) / 2.0);
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

    WeightType get_edge_list(size_t from, size_t to) const noexcept {
        for (const auto& [v, w] : adj_list_[from]) {
            if (v == to) return w;
        }
        return INF;
    }

    bool should_enable_matrix() const noexcept {
        return density() >= DENSE_THRESHOLD;
    }

    bool should_disable_matrix() const noexcept {
        return density() <= SPARSE_THRESHOLD;
    }

    void build_matrix() {
        matrix_.resize(vertices_ * vertices_, INF);
        for (size_t i = 0; i < vertices_; ++i) {
            matrix_[i * vertices_ + i] = 0;
        }

        for (size_t from = 0; from < vertices_; ++from) {
            for (const auto& [to, weight] : adj_list_[from]) {
                matrix_[from * vertices_ + to] = weight;
            }
        }

        use_matrix_ = true;
    }

    void drop_matrix() noexcept {
        containers::Vector<WeightType>().swap(matrix_);
        use_matrix_ = false;
    }

    size_t vertices_;
    size_t edges_;
    bool use_matrix_;

    using vertices_list = containers::Vector<std::pair<size_t, WeightType>>;
    containers::Vector<vertices_list> adj_list_;
    containers::Vector<WeightType> matrix_;

};

} 