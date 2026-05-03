#pragma once
#include "containers/vector.hpp"
#include <type_traits>

namespace heaps {

template <typename T, typename Compare = std::less<T>>
class BinomialHeap {
	struct Node;
public:
	using value_type = T;
	using size_type = size_t;
	using reference = T&;
	using const_reference = const T&;

	BinomialHeap() = default;

	BinomialHeap(const BinomialHeap& other)
		: root_(nullptr)
		, min_(nullptr)
		, size_(other.size_) {
		root_ = copy_tree(other.root_);
		min_ = root_;
		for (Node* curr = root_; curr; curr = curr->sibling_) {
			if (comp_(curr->value_, min_->value_)) {
				min_ = curr;
			}
		}
	}
	BinomialHeap& operator=(const BinomialHeap& other) {
		if (this == &other) return *this;
		BinomialHeap temp(other);
		swap(temp);
		return *this;
	}
	BinomialHeap(BinomialHeap&& other) noexcept
		: root_(std::exchange(other.root_, nullptr))
		, min_(std::exchange(other.min_, nullptr))
		, size_(std::exchange(other.size_, 0)) {
	}
	BinomialHeap& operator=(BinomialHeap&& other) noexcept {
		if (this == &other) return *this;
		BinomialHeap temp(std::move(other));
		swap(temp);
		return *this;
	}

	~BinomialHeap() noexcept { destroy_tree(root_); }

	void push(const T& value) { push_impl(value); }
	void push(T&& value) { push_impl(std::move(value)); }

	const_reference top() const { 
		if (empty()) throw std::runtime_error("Heap is empty");
		return min_->value_; 
	}

	void pop() {
		if (empty()) throw std::runtime_error("Heap is empty");
		
		Node* reversed_child = nullptr;
		Node* child = min_->child_;
		while (child) {
			Node* next = child->sibling_;
			child->sibling_ = reversed_child;
			child->parent_ = nullptr;
			reversed_child = child;
			child = next;
		}

		if (root_ == min_) {
			root_ = min_->sibling_;
		} else {
			Node* prev = root_;
			while (prev->sibling_ != min_) {
				prev = prev->sibling_;
			}
			prev->sibling_ = min_->sibling_;
		}

		destroy_node(min_);
		min_ = nullptr;
		--size_;

		root_ = merge_root_lists(root_, reversed_child);
		consolidate();
	}

	void merge(BinomialHeap& other) noexcept {
		if (this == &other) return;
		root_ = merge_root_lists(root_, other.root_);
		other.root_ = nullptr;
		size_ += other.size_;
		other.size_ = 0;
		other.min_ = nullptr;
		consolidate();
	}

	size_type size() const noexcept { return size_; }
	bool empty() const noexcept { return size_ == 0; }
	void clear() noexcept {
		destroy_tree(root_);
		root_ = nullptr;
		min_ = nullptr;
		size_ = 0;
	}

	void swap(BinomialHeap& other) noexcept {
		using std::swap;
		swap(root_, other.root_);
		swap(min_, other.min_);
		swap(size_, other.size_);
	}

private:


	template <typename V>
	Node* create_node(V&& val) {
		return new Node(std::forward<V>(val));
	}

	void destroy_node(Node* node) noexcept {
		delete node;
	}

	template <typename V>
	void push_impl(V&& val) {
		Node* node = create_node(std::forward<V>(val));
		++size_;
		node->sibling_ = root_;
		root_ = node;
		consolidate();
	}

	Node* merge_trees(Node* a, Node* b) noexcept {
		if (comp_(b->value_, a->value_)) {
			std::swap(a, b);
		}
		b->parent_ = a;
		b->sibling_ = a->child_;
		a->child_ = b;
		++a->degree_;
		return a;
	}

	Node* merge_root_lists(Node* a, Node* b) noexcept {
		if (!a) return b;
		if (!b) return a;

		Node* head;
		if (a->degree_ <= b->degree_) {
			head = a;
			a = a->sibling_;
		} else {
			head = b;
			b = b->sibling_;
		}

		Node* tail = head;
		while (a && b) {
			if (a->degree_ <= b->degree_) {
				tail->sibling_ = a;
				a = a->sibling_;
			} else {
				tail->sibling_ = b;
				b = b->sibling_;
			}
			tail = tail->sibling_;
		}

		tail->sibling_ = a ? a : b;
		return head;
	}

	void consolidate() {
		if (!root_) return;

		containers::Vector<Node*> degree_table(32, nullptr);

		Node* curr = root_;
		while (curr) {
			Node* next = curr->sibling_;
			curr->sibling_ = nullptr;  

			auto d = curr->degree_;
			while (d < 32 && degree_table[d]) {
				curr = merge_trees(degree_table[d], curr);
				degree_table[d] = nullptr;
				++d;
			}

			if (d >= 32) {
				degree_table.resize(d + 1, nullptr);
			}
			degree_table[d] = curr;
			curr = next;
		}

		root_ = nullptr;
		min_ = nullptr;
		for (Node* node : degree_table) {
			if (node) {
				node->sibling_ = root_;
				root_ = node;
				if (!min_ || comp_(node->value_, min_->value_)) {
					min_ = node;
				}
			}
		}
	}

	Node* copy_tree(Node* node, Node* parent = nullptr) {
		if (!node) return nullptr;
		Node* p = create_node(node->value_);
		p->degree_ = node->degree_;
		p->parent_ = parent;
		p->child_ = copy_tree(node->child_, p);
		p->sibling_ = copy_tree(node->sibling_, parent);
		return p;
	}

	void destroy_tree(Node* node) noexcept {
		if (!node) return;
		Node* child = node->child_;
		while (child) {
			Node* next = child->sibling_;
			destroy_tree(child);
			child = next;
		}
		destroy_node(node);
	}	
	
	struct Node {
		value_type value_{};
		Node* parent_{ nullptr };
		Node* child_{ nullptr };
		Node* sibling_{ nullptr };
		int32_t degree_{};
		template <typename V>
		Node(V&& val)
			: value_(std::forward<V>(val))
			, parent_(nullptr)
			, child_(nullptr)
			, sibling_(nullptr)
			, degree_(0) {
		}
	};

	Node* root_{ nullptr };
	Node* min_{ nullptr };
	Compare comp_{};
	size_type size_{};
};

}