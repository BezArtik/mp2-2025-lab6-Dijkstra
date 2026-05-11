#include "graph/graph.hpp"
#include "containers/vector.hpp"
#include "containers/stack.hpp"
#include <iostream>

namespace graph {

Graph::Graph(size_t vertices)
    : vertices_(vertices)
    , edges_(0)
    , use_matrix_(false)
    , adj_list_(vertices) {}

void Graph::add_edge(size_t from, size_t to, size_t weight) {
    validate(from, to);
    if (from == to) return;

    bool is_new_edge = add_edge_list(from, to, weight);

    if (use_matrix_) {
        matrix_[from * vertices_ + to] = weight;
        matrix_[to * vertices_ + from] = weight;
    }

    if (is_new_edge) ++edges_;

    if (!use_matrix_ && should_enable_matrix()) {
        build_matrix();
    } else if (use_matrix_ && should_disable_matrix()) {
        drop_matrix();
    }
}

const containers::Vector<std::pair<size_t, size_t>>& 
Graph::neighbors(size_t vertex) const {
    validate(vertex);
    return adj_list_[vertex];
}

size_t Graph::edge(size_t from, size_t to) const {
    validate(from, to);

    if (use_matrix_) {
        return matrix_[from * vertices_ + to];
    } else {
        return edge_list(from, to);
    }
}

bool Graph::has_edge(size_t from, size_t to) const {
    return edge(from, to) != INF;
}

bool Graph::is_connected() const {
    if (vertices_ == 0) return true;

    containers::Vector<bool> visited(vertices_, false);
    containers::Stack<size_t> stack;

    stack.push(0);
    visited[0] = true;
    size_t visited_count = 1;

    while (!stack.empty()) {
        auto vertex = stack.top();
        stack.pop();

        for (const auto& [neighbor, weight] : neighbors(vertex)) {
            if (!visited[neighbor]) {
                visited[neighbor] = true;
                stack.push(neighbor);
                ++visited_count;
            }
        }
    }

    return visited_count == vertices_;
}

size_t Graph::vertices() const noexcept { return vertices_; }
size_t Graph::edges() const noexcept { return edges_; }

void Graph::validate(size_t v) const {
    if (v >= vertices_) throw std::out_of_range("Vertex index out of range");
}

void Graph::validate(size_t from, size_t to) const {
    validate(from);
    validate(to);
}

double Graph::density() const noexcept {
    if (vertices_ < 2) return 0.0;
    return static_cast<double>(edges_) / (vertices_ * (vertices_ - 1) / 2.0);
}

bool Graph::add_edge_list(size_t from, size_t to, size_t weight) {
    for (auto& [v, w] : adj_list_[from]) {
        if (v == to) {
            w = weight;
            for (auto& [v2, w2] : adj_list_[to]) {
                if (v2 == from) { w2 = weight; return false; }
            }
            return false;
        }
    }
    adj_list_[from].push_back({ to, weight });
    adj_list_[to].push_back({ from, weight });
    return true;
}

size_t Graph::edge_list(size_t from, size_t to) const noexcept {
    for (const auto& [v, w] : adj_list_[from]) {
        if (v == to) return w;
    }
    return INF;
}

bool Graph::should_enable_matrix() const noexcept {
    return density() >= DENSE_THRESHOLD;
}

bool Graph::should_disable_matrix() const noexcept {
    return density() <= SPARSE_THRESHOLD;
}

void Graph::build_matrix() {
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

void Graph::drop_matrix() noexcept {
    containers::Vector<size_t>().swap(matrix_);
    use_matrix_ = false;
}

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