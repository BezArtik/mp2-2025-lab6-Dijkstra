#include "test_common/test_common_heaps.hpp"
#include "heaps/d_heap.hpp"
#include <string>

namespace tests {

template <typename T>
using DHeap2 = heaps::DHeap<T, 2>;
template <typename T>
using DHeap3 = heaps::DHeap<T, 3>;

using DHeapTypes = ::testing::Types<
    DHeap2<int>, DHeap2<double>, DHeap2<std::string>,
    DHeap3<int>, DHeap3<double>, DHeap3<std::string>
>;

INSTANTIATE_TYPED_TEST_SUITE_P(DHeap, HeapTest, DHeapTypes);

}
