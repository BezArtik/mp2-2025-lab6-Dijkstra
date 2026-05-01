#pragma once
#include "containers/vector.hpp"
#include <concepts>
#include <type_traits>

namespace heaps {

template <typename T, int8_t d = 3, typename Compare = std::less<T>>
class DHeap {
public:
	using value_type = T;
	using size_type = size_t;
	using reference = T&;
	using const_reference = const T&;

	DHeap() = default;

	template <std::input_iterator InputIt>
	DHeap(InputIt first, InputIt last) : data_(first, last) { make_heap(); }

	void push(const_reference value) {
		data_.push_back(value);
		sift_up(data_.size() - 1);
	}
	void push(value_type&& value) {
		data_.push_back(std::move(value));
		sift_up(data_.size() - 1);
	}
	void pop() {
		if (data_.empty()) return;
		std::swap(data_.front(), data_.back());
		data_.pop_back();
		if (!data_.empty()) {
			sift_down(0);
		}
	}
	const_reference top() const noexcept { return data_.front(); }
	bool empty() const noexcept { return data_.empty(); }
	size_type size() const noexcept { return data_.size(); }

private:
	containers::Vector<value_type> data_{};
	Compare comp_{};

	size_type parent(size_type index) const noexcept { return (index - 1) / d; }
	size_type child(size_type index, size_type k) const noexcept { return d * index + k + 1; }

	void sift_up(size_type index) {
		if (index == 0) return;
		auto value = std::move(data_[index]);

		while (index > 0) {
			const auto p = parent(index);
			if (!comp_(value, data_[p])) break;
			data_[index] = std::move(data_[p]);
			index = p;
		}
		data_[index] = std::move(value);
	}

	void sift_down(size_type index) {
		const auto n = data_.size();
		auto value = std::move(data_[index]);

		while (true) {
			const auto first = child(index, 0);
			if (first >= n) break;
			auto best = first;
			const auto last = std::min(child(index, d - 1), n - 1);
			for (size_type c = first + 1; c <= last; ++c) {
				if (comp_(data_[c], data_[best])) {
					best = c;
				}
			}
			if (!comp_(data_[best], value)) break;
			data_[index] = std::move(data_[best]);
			index = best;
		}
		data_[index] = std::move(value);
	}

	void make_heap() {
		const auto n = data_.size();
		if (n <= 1) return;
		const auto last_parent = (n - 2) / d;
		for (size_type i = last_parent + 1; i-- > 0;) {
			sift_down(i);
		}
	}
};

}