#pragma once
#include <random>
#include <type_traits>

namespace gen {

class StringGenerator {
public:
    StringGenerator(size_t min_len = 50, size_t max_len = 100);
    std::string operator()();

private:
    std::mt19937_64 gen_;
    std::uniform_int_distribution<size_t> len_dis_;
    std::uniform_int_distribution<size_t> char_dis_;
    const char* chars_ = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
};

template <typename T>
struct Generator {
    T operator()() const {
        if constexpr (std::is_same_v<T, std::string>) {
            return StringGenerator{}();
        } else if constexpr (std::is_same_v<T, int>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_int_distribution<int> dist(-10'000, 10'000);
            return dist(gen);
        } else if constexpr (std::is_same_v<T, double>) {
            static std::mt19937_64 gen(std::random_device{}());
            static std::uniform_real_distribution<double> dist(-10'000.0, 10'000.0);
            return dist(gen);
        } else {
            return T{};
        }
    }
};

}