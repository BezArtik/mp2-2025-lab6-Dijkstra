#include "generators.hpp"
#include <random>
#include <string>
#include <iterator>
#include <algorithm>

namespace gen {

StringGenerator::StringGenerator(size_t min_len, size_t max_len)
    : gen_(std::random_device{}())
    , len_dis_(min_len, max_len)
    , char_dis_(0, sizeof(chars_) - 2) {
}

std::string StringGenerator::operator()() {
    size_t len = len_dis_(gen_);
    std::string res;
    res.reserve(len);
    std::generate_n(std::back_inserter(res), len,
        [&]() { return chars_[char_dis_(gen_)];
        });
    return res;
}

}