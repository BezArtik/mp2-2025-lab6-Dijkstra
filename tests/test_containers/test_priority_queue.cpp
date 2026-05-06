#include "test_common/test_common_containers.hpp"
#include "containers/priority_queue.hpp"
#include <gtest/gtest.h>
#include <utility>
#include <algorithm>

namespace tests {

template<typename T>
class PriorityQueueTest : public ContainerTest<T> {};

TYPED_TEST_SUITE(PriorityQueueTest, FunctionalTypes);

TYPED_TEST(PriorityQueueTest, can_create_empty) {
    ASSERT_NO_THROW(containers::PriorityQueue<TypeParam> pq);
    containers::PriorityQueue<TypeParam> pq;
    EXPECT_TRUE(pq.empty());
}

TYPED_TEST(PriorityQueueTest, can_copy) {
    containers::PriorityQueue<TypeParam> pq1;
    auto obj = this->create();
    pq1.push(obj);

    containers::PriorityQueue<TypeParam> pq2(pq1);
    ASSERT_EQ(pq2.size(), 1);
    ASSERT_EQ(pq2.top(), obj);
    ASSERT_EQ(pq1.size(), 1);
}

TYPED_TEST(PriorityQueueTest, can_move) {
    containers::PriorityQueue<TypeParam> pq1;
    auto obj = this->create();
    pq1.push(obj);

    containers::PriorityQueue<TypeParam> pq2(std::move(pq1));
    ASSERT_EQ(pq2.size(), 1);
    ASSERT_EQ(pq2.top(), obj);
    EXPECT_TRUE(pq1.empty());
}

TYPED_TEST(PriorityQueueTest, can_push_elements) {
    containers::PriorityQueue<TypeParam> pq;
    auto obj = this->create();

    ASSERT_NO_THROW(pq.push(obj));
    ASSERT_EQ(pq.size(), 1);
    ASSERT_EQ(pq.top(), obj);
}

TYPED_TEST(PriorityQueueTest, can_push_move) {
    containers::PriorityQueue<TypeParam> pq;
    auto obj = this->create();

    ASSERT_NO_THROW(pq.push(std::move(obj)));
    ASSERT_EQ(pq.size(), 1);
}

TYPED_TEST(PriorityQueueTest, can_pop_elements) {
    containers::PriorityQueue<TypeParam> pq;
    auto obj = this->create();
    pq.push(obj);
    ASSERT_NO_THROW(pq.pop());
    EXPECT_TRUE(pq.empty());
}

}