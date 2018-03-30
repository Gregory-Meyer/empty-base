#include "pair.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace std {

template <typename CharT, typename Traits, typename T>
basic_ostream<CharT, Traits>&
operator<<(basic_ostream<CharT, Traits> &os, const vector<T> &vector) {
    os << '[';

    for (auto iter = vector.cbegin(); iter != vector.cend(); ++iter) {
        if (iter - vector.cbegin() != 0) {
            os << ", ";
        }

        os << *iter;
    }

    return os << ']';
}

} // namespace std

template <typename CharT, typename Traits>
std::basic_ostream<CharT, Traits>&
nl(std::basic_ostream<CharT, Traits> &os) {
    return os << '\n';
}

void std_test() {
    std::cout << "std_test" << nl;

    gregjm::Pair<std::vector<int>, std::string> pair{
        std::piecewise_construct, std::forward_as_tuple(15, 0),
        std::forward_as_tuple("foo bar")
    };

    std::cout << "sizeof(pair) = " << sizeof(pair) << nl;
    std::cout << "sizeof(std::pair) = "
              << sizeof(std::pair<std::vector<int>, std::string>) << nl;

    decltype(pair) pair2 = std::move(pair);

    std::cout << "pair = " << pair << nl;
    std::cout << "pair2 = " << pair2 << nl;

    pair.swap(pair2);

    std::cout << "pair = " << pair << nl;
    std::cout << "pair2 = " << pair2 << nl;

    pair2 = pair;

    std::cout << "pair < pair2 = " << (pair < pair2) << nl;

    std::cout << "pair = " << pair << nl;
    std::cout << "pair2 = " << pair2 << nl;

    std::cout << "end std_test" << nl << nl;
}

void fundamental_test() {
    std::cout << "fundamental_test" << nl;

    gregjm::Pair<int, double> pair{ 0, 0.0 };

    std::cout << "pair = " << pair << nl;
    std::cout << "first = " << pair.first() << nl;
    std::cout << "second = " << pair.second() << nl;

    std::cout << "end fundamental_test" << nl << nl;
}

void reference_test() {
    std::cout << "reference_test" << nl;

    int i = 0;
    double d = 15;

    gregjm::Pair<int&, double&> pair{ i, d };

    std::cout << "pair = " << pair << nl;
    std::cout << "first = " << pair.first() << nl;
    std::cout << "second = " << pair.second() << nl;

    std::cout << "end reference_test" << nl << nl;
}

void pointer_test() {
    std::cout << "pointer_test" << nl;

    gregjm::Pair<int*, double*> pair{ nullptr, nullptr };

    std::cout << "pair = " << pair << nl;
    std::cout << "first = " << pair.first() << nl;
    std::cout << "second = " << pair.second() << nl;

    std::cout << "end pointer_test" << nl << nl;
}

void empty_test() {
    struct Empty { };

    std::cout << "empty_test" << nl;

    gregjm::Pair<Empty, Empty> pair;

    std::cout << "sizeof(pair) = " << sizeof(pair) << nl;

    std::cout << "end empty_test" << nl << nl;
}

void final_test() {
    struct Final final { };

    std::cout << "final_test" << nl;

    gregjm::Pair<int, Final> pair;

    std::cout << "sizeof(pair) = " << sizeof(pair) << nl;

    std::cout << "end final_test" << nl << nl;
}

void union_test() {
    union Union {
        int i;
        float f;
    };

    std::cout << "union_test" << nl;

    gregjm::Pair<int, Union> pair;

    pair.first() = 15;
    pair.second().f = 5.0f;

    std::cout << "pair.first() = " << pair.first() << nl;
    std::cout << "pair.second().i = " << pair.second().i << nl;
    std::cout << "pair.second().f = " << pair.second(). f<< nl;
    std::cout << "sizeof(pair) = " << sizeof(pair) << nl;

    std::cout << "end union_test" << nl << nl;
}

void copy_move_test() {
    std::cout << "copy_move_test" << nl;

    gregjm::Pair<int, int> ipair{ 15, 30 };
    gregjm::Pair<double, double> dpair{ ipair };
    gregjm::Pair<double, double> dpair2{ std::move(ipair) };

    std::cout << "ipair = " << ipair << nl;
    std::cout << "dpair = " << dpair << nl;
    std::cout << "dpair2 = " << dpair2 << nl;

    std::cout << "ipair < dpair = " << (ipair < dpair) << nl;

    std::cout << "end copy_move_test" << nl << nl;
}

int main() {
    fundamental_test();
    std_test();
    reference_test();
    pointer_test();
    empty_test();
    final_test();
    union_test();
    copy_move_test();
}
