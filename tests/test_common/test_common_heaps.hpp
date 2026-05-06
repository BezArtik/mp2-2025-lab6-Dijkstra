#pragma once
#include "containers/vector.hpp"
#include "generators/generators.hpp"
#include <gtest/gtest.h>
#include <utility>
#include <algorithm>
#include <iterator>

namespace tests {

template <typename HeapType>
class HeapTest : public ::testing::Test {
protected:
    using Heap = HeapType;
    using T = typename Heap::value_type;

    Heap heap_;

    auto create_sequence(size_t n) const {
        containers::Vector<T> result(n);
        std::generate(result.begin(), result.end(), gen::Generator<T>{});
        return result;
    }

    template <std::input_iterator Iter>
    void push(Iter begin, Iter end) {
        for (; begin != end; ++begin) {
            heap_.push(*begin);
        }
    }

    bool verify_min_heap() const {
        if (heap_.empty()) return true;
        containers::Vector<T> elements;
        auto temp_heap = heap_;
        while (!temp_heap.empty()) {
            elements.push_back(temp_heap.top());
            temp_heap.pop();
        }
        return std::is_sorted(elements.begin(), elements.end());
    }

};


TYPED_TEST_SUITE_P(HeapTest);

TYPED_TEST_P(HeapTest, create_empty) {
    ASSERT_NO_THROW(TypeParam heap);
    TypeParam heap;
    EXPECT_TRUE(heap.empty());
    EXPECT_EQ(heap.size(), 0);
}

TYPED_TEST_P(HeapTest, push_elements) {
    auto elements = this->create_sequence(10);

    for (const auto& elem : elements) {
        ASSERT_NO_THROW(this->heap_.push(elem));
    }

    EXPECT_EQ(this->heap_.size(), elements.size());
    EXPECT_FALSE(this->heap_.empty());
}

TYPED_TEST_P(HeapTest, push_move_elements) {
    auto elements = this->create_sequence(5);

    for (auto& elem : elements) {
        auto moved = std::move(elem);
        ASSERT_NO_THROW(this->heap_.push(std::move(moved)));
    }

    EXPECT_EQ(this->heap_.size(), 5);
}

TYPED_TEST_P(HeapTest, top_returns_minimum) {
    auto elements = this->create_sequence(20);
    this->push(elements.begin(), elements.end());

    auto min_element = *std::min_element(elements.begin(), elements.end());
    EXPECT_EQ(this->heap_.top(), min_element);
}

TYPED_TEST_P(HeapTest, pop_removes_minimum) {
    auto elements = this->create_sequence(15);
    this->push(elements.begin(), elements.end());

    std::sort(elements.begin(), elements.end());

    for (const auto& expected : elements) {
        ASSERT_FALSE(this->heap_.empty());
        EXPECT_EQ(this->heap_.top(), expected);
        this->heap_.pop();
    }

    EXPECT_TRUE(this->heap_.empty());
}

TYPED_TEST_P(HeapTest, property_maintained) {
    auto elements = this->create_sequence(30);
    this->push(elements.begin(), elements.end());

    EXPECT_TRUE(this->verify_min_heap());
}

TYPED_TEST_P(HeapTest, clear) {
    auto elements = this->create_sequence(10);
    this->push(elements.begin(), elements.end());

    ASSERT_NO_THROW(this->heap_.clear());
    EXPECT_TRUE(this->heap_.empty());
    EXPECT_EQ(this->heap_.size(), 0);
}

TYPED_TEST_P(HeapTest, copy_constructor) {
    auto elements = this->create_sequence(10);
    this->push(elements.begin(), elements.end());

    TypeParam heap_copy(this->heap_);

    EXPECT_EQ(heap_copy.size(), this->heap_.size());

    while (!this->heap_.empty()) {
        EXPECT_EQ(heap_copy.top(), this->heap_.top());
        heap_copy.pop();
        this->heap_.pop();
    }

    EXPECT_TRUE(heap_copy.empty());
}

TYPED_TEST_P(HeapTest, move_constructor) {
    auto elements = this->create_sequence(10);
    this->push(elements.begin(), elements.end());

    auto original_size = this->heap_.size();
    auto original_top = this->heap_.top();

    TypeParam heap_moved(std::move(this->heap_));

    EXPECT_EQ(heap_moved.size(), original_size);
    EXPECT_EQ(heap_moved.top(), original_top);
    EXPECT_TRUE(this->heap_.empty());
}

TYPED_TEST_P(HeapTest, move_assignment) {
    auto elements = this->create_sequence(10);
    this->push(elements.begin(), elements.end());

    TypeParam heap2;
    auto elements2 = this->create_sequence(5);
    for (const auto& elem : elements2) {
        heap2.push(elem);
    }

    auto original_size = this->heap_.size();
    auto original_top = this->heap_.top();

    heap2 = std::move(this->heap_);

    EXPECT_EQ(heap2.size(), original_size);
    EXPECT_EQ(heap2.top(), original_top);
    EXPECT_TRUE(this->heap_.empty());
}

TYPED_TEST_P(HeapTest, swap) {
    auto elements1 = this->create_sequence(10);
    this->push(elements1.begin(), elements1.end());

    TypeParam heap2;
    auto elements2 = this->create_sequence(5);
    for (const auto& elem : elements2) {
        heap2.push(elem);
    }

    auto size1 = this->heap_.size();
    auto size2 = heap2.size();
    auto top1 = this->heap_.top();
    auto top2 = heap2.top();

    this->heap_.swap(heap2);

    EXPECT_EQ(this->heap_.size(), size2);
    EXPECT_EQ(this->heap_.top(), top2);
    EXPECT_EQ(heap2.size(), size1);
    EXPECT_EQ(heap2.top(), top1);
}

template <typename HeapType>
class MergeableHeapTest : public HeapTest<HeapType> {};

TYPED_TEST_SUITE_P(MergeableHeapTest);

TYPED_TEST_P(MergeableHeapTest, merge) {
    auto elements1 = this->create_sequence(10);
    this->push(elements1.begin(), elements1.end());

    TypeParam heap2;
    auto elements2 = this->create_sequence(10);
    for (const auto& elem : elements2) {
        heap2.push(elem);
    }

    auto total_size = this->heap_.size() + heap2.size();

    this->heap_.merge(heap2);

    EXPECT_EQ(this->heap_.size(), total_size);
    EXPECT_TRUE(heap2.empty());
    EXPECT_TRUE(this->verify_min_heap());
}

TYPED_TEST_P(MergeableHeapTest, merge_empty) {
    TypeParam heap2;

    ASSERT_NO_THROW(this->heap_.merge(heap2));
    EXPECT_TRUE(this->heap_.empty());

    heap2.merge(this->heap_);
    EXPECT_TRUE(heap2.empty());
}

TYPED_TEST_P(MergeableHeapTest, merge_with_self) {
    auto elements = this->create_sequence(5);
    this->push(elements.begin(), elements.end());

    auto original_size = this->heap_.size();
    ASSERT_NO_THROW(this->heap_.merge(this->heap_));
    EXPECT_EQ(this->heap_.size(), original_size);
}

REGISTER_TYPED_TEST_SUITE_P(HeapTest,
    create_empty,
    push_elements,
    push_move_elements,
    top_returns_minimum,
    pop_removes_minimum,
    property_maintained,
    clear,
    copy_constructor,
    move_constructor,
    move_assignment,
    swap
);

REGISTER_TYPED_TEST_SUITE_P(MergeableHeapTest,
    merge,
    merge_empty,
    merge_with_self
);

}