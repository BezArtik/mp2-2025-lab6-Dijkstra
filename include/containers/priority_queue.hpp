#pragma once
#include "heaps/d_heap.hpp"

namespace containers {

template <typename T, 
	      typename Container = heaps::DHeap<T>, 
	      typename Compare = std::less<T>>
class PriorityQueue {
public:

	using container_type = Container;
	using value_type = typename Container::value_type;
	using size_type = typename Container::size_type;
	using reference = typename Container::reference;
	using const_reference = typename Container::const_reference;

	const_reference top() const { return container_.top(); }
	bool empty() const noexcept { return container_.empty(); }
	size_type size() const noexcept { return container_.size(); }
	void push(const_reference value) { container_.push(value); }
	void push(value_type&& value) { container_.push(std::move(value)); }
	void pop() { container_.pop(); }

private:
	container_type container_;
	Compare comp_;
}

}