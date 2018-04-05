#include "pair.hpp"

#include "catch.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

struct Empty { };

struct Final final {
    int i;
};

struct NonDefaultConstructible {
    int i;

    constexpr NonDefaultConstructible(int x) noexcept : i{ x } { }
};

TEST_CASE("Pair objects can be instantiated with builtin types", "[Pair]") {
    SECTION("int pair") {
        gregjm::Pair<int, int> pair{ 0, 5 };

        REQUIRE(pair.first() == 0);
        REQUIRE(pair.second() == 5);

        pair.first() = 15;

        REQUIRE(pair.first() == 15);
        REQUIRE(pair.second() == 5);

        pair.second() = 10;

        REQUIRE(pair.first() == 15);
        REQUIRE(pair.second() == 10);

        pair.first() = 20;
        pair.second() = 25;

        REQUIRE(pair.first() == 20);
        REQUIRE(pair.second() == 25);

        using std::swap;

        swap(pair.first(), pair.second());

        REQUIRE(pair.first() == 25);
        REQUIRE(pair.second() == 20);
    }

    SECTION("double pair") {
        gregjm::Pair<double, double> pair{ 0.0, 5.0 };

        REQUIRE(pair.first() == 0.0);
        REQUIRE(pair.second() == 5.0);

        pair.first() = 15.0;

        REQUIRE(pair.first() == 15.0);
        REQUIRE(pair.second() == 5.0);

        pair.second() = 10.0;

        REQUIRE(pair.first() == 15.0);
        REQUIRE(pair.second() == 10.0);

        pair.first() = 20.0;
        pair.second() = 25.0;

        REQUIRE(pair.first() == 20.0);
        REQUIRE(pair.second() == 25.0);

        using std::swap;

        swap(pair.first(), pair.second());

        REQUIRE(pair.first() == 25.0);
        REQUIRE(pair.second() == 20.0);
    }

    SECTION("char* pair") {
        using namespace std::literals;

        gregjm::Pair<const char*, const char*> pair{ "foo", "bar" };

        REQUIRE_THAT(pair.first(), Catch::Matchers::Equals("foo"));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("bar"));

        pair.first() = "baz";

        REQUIRE_THAT(pair.first(), Catch::Matchers::Equals("baz"));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("bar"));

        pair.second() = "qux";

        REQUIRE_THAT(pair.first(), Catch::Matchers::Equals("baz"));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("qux"));

        pair.first() = "ayy";
        pair.second() = "lmao";

        REQUIRE_THAT(pair.first(), Catch::Matchers::Equals("ayy"));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("lmao"));

        using std::swap;

        swap(pair.first(), pair.second());

        REQUIRE_THAT(pair.first(), Catch::Matchers::Equals("lmao"));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("ayy"));
    }
}

TEST_CASE("Pair objects can be instantiated in fun ways", "[Pair") {
    REQUIRE(true);
}

// int main() {
//     fundamental_test();
//     std_test();
//     reference_test();
//     pointer_test();
//     empty_test();
//     final_test();
//     union_test();
//     copy_move_test();
// }
