#include "test_common/test_common_heaps.hpp"
#include "heaps/binomial_heap.hpp"
#include <string>

namespace tests {

using BinomialHeapTypes = ::testing::Types<
    heaps::BinomialHeap<int>,
    heaps::BinomialHeap<double>,
    heaps::BinomialHeap<std::string>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(BinomialHeap, HeapTest, BinomialHeapTypes);
INSTANTIATE_TYPED_TEST_SUITE_P(BinomialHeap, MergeableHeapTest, BinomialHeapTypes);

}