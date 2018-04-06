#include "pair.hpp"

#include "catch.hpp"

#include <functional>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

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

TEST_CASE("Pair objects are constructed with perfect forwarding", "[Pair]") {
    SECTION("single argument forwarding") {
        const std::initializer_list<char> init = { 'f', 'o', 'o' };
        const gregjm::Pair<std::vector<int>, std::string> pair{ 3, init };

        REQUIRE_THAT(pair.first(),
                     Catch::Matchers::Equals(std::vector<int>{ 0, 0, 0 }));
        REQUIRE_THAT(pair.second(), Catch::Matchers::Equals("foo"));
    }

    SECTION("multi argument forwarding") {
        using PairT = gregjm::Pair<std::vector<int>, std::tuple<int, double>>;

        const PairT pair{ std::piecewise_construct,
                          std::forward_as_tuple(3, 1),
                          std::forward_as_tuple(15, 3.0) };

        REQUIRE_THAT(pair.first(),
                     Catch::Matchers::Equals(std::vector<int>{ 1, 1, 1 }));
        REQUIRE(std::get<0>(pair.second()) == 15);
        REQUIRE(std::get<1>(pair.second()) == 3.0);
    }

    SECTION("multi argument forwarding, again") {
        using PairT = gregjm::Pair<std::vector<int>, int>;

        const std::set<int> numbers = { 0, 1, 2 };

        const PairT pair{ std::piecewise_construct,
                          std::forward_as_tuple(numbers.cbegin(),
                                                numbers.cend()),
                          std::forward_as_tuple(0) };

        REQUIRE_THAT(pair.first(),
                     Catch::Matchers::Equals(std::vector<int>{ 0, 1, 2 }));
        REQUIRE(pair.second() == 0);
    }
}

TEST_CASE("Pair objects support all sorts of strange types", "[Pair]") {
    SECTION("empty type") {
        struct Empty {
            Empty() = default;
        };

        const gregjm::Pair<Empty, int> pair{ std::piecewise_construct,
                                             std::forward_as_tuple(),
                                             std::forward_as_tuple(0) };

        REQUIRE(sizeof(pair) == sizeof(int));
    }

    SECTION("final type") {
        struct Final final {
            Final() = default;

            int i;
        };

        const gregjm::Pair<Final, int> pair{ std::piecewise_construct,
                                             std::forward_as_tuple(),
                                             std::forward_as_tuple(0) };

        REQUIRE(pair.first().i == 0);
        REQUIRE(pair.second() == 0);
    }

    SECTION("reference type") {
        int i = 15;
        double d = 3.0;

        const gregjm::Pair<int&, double&> pair{ i, d };

        REQUIRE(pair.first() == 15);
        REQUIRE(&pair.first() == &i);
        REQUIRE(pair.second() == 3.0);
        REQUIRE(&pair.second() == &d);
    }

    SECTION("pointer type") {
        int i = 15;
        double d = 3.0;

        const gregjm::Pair<int*, double*> pair{ &i, &d };

        REQUIRE(*pair.first() == 15);
        REQUIRE(pair.first() == &i);
        REQUIRE(*pair.second() == 3.0);
        REQUIRE(pair.second() == &d);
    }

    SECTION("const type") {
        const gregjm::Pair<const int, int> pair{ 1, 2 };

        REQUIRE(pair.first() == 1);
        REQUIRE(pair.second() == 2);
    }

    SECTION("volatile type") {
        const gregjm::Pair<volatile int, int> pair{ 1, 2 };

        REQUIRE(pair.first() == 1);
        REQUIRE(pair.second() == 2);
    }

    SECTION("union type") {
        union Union {
            constexpr Union(int value) noexcept : i{ value } { };

            int i;
            double d;
        };

        gregjm::Pair<Union, int> pair{ 0, 0 };

        REQUIRE(pair.first().i == 0);
        REQUIRE(pair.second() == 0);

        pair.first().d = 8.0;

        REQUIRE(pair.first().d == 8.0);
        REQUIRE(pair.second() == 0);
    }

    SECTION("constexpr types") {
        struct Constexpr {
            constexpr Constexpr() noexcept = default;

            int i = 0;
        };

        constexpr gregjm::Pair<Constexpr, Constexpr> pair;

        REQUIRE(pair.first().i == 0);
        REQUIRE(pair.second().i == 0);

        REQUIRE(&pair.first() != &pair.second());
    }
}

TEST_CASE("Pair types can be constructed from non identical pairs", "[Pair]") {
    SECTION("copy constuction") {
        struct Type { };

        struct CopyableFromType {
            constexpr CopyableFromType(const Type&) noexcept { }
        };

        struct NotCopyableFromType { };

        REQUIRE(std::is_constructible_v<
            gregjm::Pair<CopyableFromType, CopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_constructible_v<
            gregjm::Pair<NotCopyableFromType, NotCopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_constructible_v<
            gregjm::Pair<CopyableFromType, NotCopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_constructible_v<
            gregjm::Pair<NotCopyableFromType, CopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);
    }

    SECTION("move construction") {
        struct Type { };

        struct MovableFromType {
            constexpr MovableFromType(Type&&) noexcept { }
        };

        struct NotMovableFromType { };

        REQUIRE(std::is_constructible_v<
            gregjm::Pair<MovableFromType, MovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);

        REQUIRE_FALSE(std::is_constructible_v<
            gregjm::Pair<NotMovableFromType, MovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);

        REQUIRE_FALSE(std::is_constructible_v<
            gregjm::Pair<MovableFromType, NotMovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);
    }
}

TEST_CASE("Pair types can be assigned from non identical pairs", "[Pair]") {
    SECTION("copy constuction") {
        struct Type { };

        struct CopyableFromType {
            constexpr CopyableFromType& operator=(const Type&) noexcept {
                return *this;
            }
        };

        struct NotCopyableFromType { };

        REQUIRE(std::is_assignable_v<
            gregjm::Pair<CopyableFromType, CopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_assignable_v<
            gregjm::Pair<NotCopyableFromType, NotCopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_assignable_v<
            gregjm::Pair<CopyableFromType, NotCopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);

        REQUIRE_FALSE(std::is_assignable_v<
            gregjm::Pair<NotCopyableFromType, CopyableFromType>,
            const gregjm::Pair<Type, Type>&
        >);
    }

    SECTION("move construction") {
        struct Type { };

        struct MovableFromType {
            constexpr MovableFromType& operator=(Type&&) noexcept {
                return *this;
            }
        };

        struct NotMovableFromType { };

        REQUIRE(std::is_assignable_v<
            gregjm::Pair<MovableFromType, MovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);

        REQUIRE_FALSE(std::is_assignable_v<
            gregjm::Pair<NotMovableFromType, MovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);

        REQUIRE_FALSE(std::is_assignable_v<
            gregjm::Pair<MovableFromType, NotMovableFromType>,
            gregjm::Pair<Type, Type>&&
        >);
    }
}

TEST_CASE("Type traits carry through to Pair", "[Pair]") {
    SECTION("trivially destructible") {
        REQUIRE(std::is_trivially_destructible_v<gregjm::Pair<int, int>>);
        REQUIRE_FALSE(std::is_trivially_destructible_v<
            gregjm::Pair<std::vector<int>, int>
        >);
        REQUIRE_FALSE(std::is_trivially_destructible_v<
            gregjm::Pair<int, std::vector<int>>
        >);
    }

    SECTION("nothrow constructible") {
        struct NothrowConstructible {
            constexpr NothrowConstructible() noexcept { };
        };

        struct ThrowConstructible {
            constexpr ThrowConstructible() { };
        };

        REQUIRE(std::is_nothrow_default_constructible_v<
            gregjm::Pair<NothrowConstructible, NothrowConstructible>
        >);
        REQUIRE_FALSE(std::is_nothrow_default_constructible_v<
            gregjm::Pair<ThrowConstructible, NothrowConstructible>
        >);
        REQUIRE_FALSE(std::is_nothrow_default_constructible_v<
            gregjm::Pair<NothrowConstructible, ThrowConstructible>
        >);
    }
}
