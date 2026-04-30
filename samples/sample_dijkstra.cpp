#include "heaps/binomial_heap.hpp"
#include "heaps/d_heap.hpp"
#include <iostream>
#include <exception>

int main() {
	try {
		heaps::DHeap<int, 3> d_heap; 
		d_heap.push(10);
		d_heap.push(5);
		d_heap.push(20);
		d_heap.push(15);
		std::cout << "Top element: " << d_heap.top() << std::endl; 
		d_heap.pop();
		std::cout << "Top element after pop: " << d_heap.top() << std::endl; 
		heaps::BinomialHeap<int> binomial_heap;
		binomial_heap.push(30);
		binomial_heap.push(25);
		binomial_heap.push(40);
		std::cout << "Top element of binomial heap: " << binomial_heap.top() << std::endl; 
	} catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
	}
    return 0;
}