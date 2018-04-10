#include "pair.hpp"

#include <algorithm>
#include <chrono>
#include <functional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

template <typename I, typename ...As, typename =
              std::enable_if_t<std::is_invocable_v<I, As...>>>
std::chrono::nanoseconds time(I &&invocable, As &&...args) {
    const auto start = std::chrono::high_resolution_clock::now();
    std::invoke(std::forward<I>(invocable), std::forward<As>(args)...);
    const auto end = std::chrono::high_resolution_clock::now();

    return end - start;
}

template <typename CharT, typename Traits, typename T>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits> &os, const std::vector<T> &vec) {
    os << '[';

    {
    bool is_first = true;
    for (const auto &elem : vec) {
        if (is_first) {
            is_first = false;
        } else {
            os << ", ";
        }

        os << elem;
    }
    }

    return os << ']';
}

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>& nl(std::basic_ostream<CharT, Traits> &os) {
    return os << '\n';
}

std::string std_vector_bench() {
    std::ostringstream oss;
    std::pair<std::vector<int>, std::vector<int>> pair;

    for (int i = 0; i < 64; ++i) {
        pair.first.push_back(i);
        pair.second.insert(pair.second.cbegin(), i);

        oss << pair.first << ", " << pair.second << nl;
    }

    return oss.str();
}

std::string ebo_vector_bench() {
    std::ostringstream oss;
    gregjm::Pair<std::vector<int>, std::vector<int>> pair;

    for (int i = 0; i < 64; ++i) {
        pair.first().push_back(i);
        pair.second().insert(pair.second().cbegin(), i);

        oss << pair.first() << ", " << pair.second() << nl;
    }

    return oss.str();
}

std::string std_size_t_bench() {
    const std::hash<std::string> hash;

    std::ostringstream oss;
    std::string to_hash = "this is a hashable string";
    std::pair<std::size_t, std::size_t> pair;

    for (int i = 0; i < 512; ++i) {
        pair.first = hash(to_hash);
        std::next_permutation(to_hash.begin(), to_hash.end());
        pair.second = hash(to_hash);
        std::next_permutation(to_hash.begin(), to_hash.end());

        oss << pair.first << ", " << pair.second << nl;
    }

    return oss.str();
}

std::string ebo_size_t_bench() {
    const std::hash<std::string> hash;

    std::ostringstream oss;
    std::string to_hash = "this is a hashable string";
    gregjm::Pair<std::size_t, std::size_t> pair;

    for (int i = 0; i < 512; ++i) {
        pair.first() = hash(to_hash);
        std::next_permutation(to_hash.begin(), to_hash.end());
        pair.second() = hash(to_hash);
        std::next_permutation(to_hash.begin(), to_hash.end());

        oss << pair.first() << ", " << pair.second() << nl;
    }

    return oss.str();
}

int main() {
    for (int i = 0; i < 512; ++i) {
        const auto std_size_t_ns = time(std_size_t_bench).count();
        const auto ebo_size_t_ns = time(ebo_size_t_bench).count();

        const auto std_vector_ns = time(std_vector_bench).count();
        const auto ebo_vector_ns = time(ebo_vector_bench).count();

        std::cout << std_size_t_ns << ", " << ebo_size_t_ns << ", "
                  << std_vector_ns << ", " << ebo_vector_ns << nl;
    }
}
