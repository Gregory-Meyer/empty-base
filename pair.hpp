#ifndef GREGJM_PAIR_HPP
#define GREGJM_PAIR_HPP

#include "pair_detail.hpp"

#include <cstddef> // std::size_t
#include <iostream> // std::basic_ostream
#include <tuple> // std::tuple_size_v, std::get
#include <type_traits>
#include <utility> // std::forward, std::pair, std::declval, std::swap

namespace gregjm {

template <typename First, typename Second>
class Pair
: private detail::WrapIfNotInheritableT<First, 0>,
  private detail::WrapIfNotInheritableT<Second, 1> {
private:
    using FirstT = detail::WrapIfNotInheritableT<First, 0>;
    using SecondT = detail::WrapIfNotInheritableT<Second, 1>;

public:
    using first_type = First;
    using second_type = Second;

    constexpr Pair()
    noexcept(std::is_nothrow_default_constructible_v<First>
             && std::is_nothrow_default_constructible_v<Second>) = default;

    template <typename T, typename U,
              typename = std::enable_if_t<
                  std::is_constructible_v<First, const T&>
                  && std::is_constructible_v<Second, const U&>
              >>
    explicit constexpr Pair(const Pair<T, U> &other)
    noexcept(std::is_nothrow_constructible_v<First, const T&>
             && std::is_nothrow_constructible_v<Second, const U&>)
    : FirstT(other.first()), SecondT(other.second()) { }

    template <typename T, typename U,
              typename =
                  std::enable_if_t<std::is_constructible_v<First, T&&>
                                   && std::is_constructible_v<Second, U&&>>>
    explicit constexpr Pair(Pair<T, U> &&other)
    noexcept(std::is_nothrow_constructible_v<First, T&&>
             && std::is_nothrow_constructible_v<Second, U&&>)
    : FirstT(std::move(other.first())),
      SecondT(std::move(other.second())) { }

    template <typename F, typename S,
              typename = std::enable_if_t<
                  std::is_constructible_v<First, F>
                  && std::is_constructible_v<Second, S>
              >>
    constexpr Pair(F &&first, S &&second)
    noexcept(std::is_nothrow_constructible_v<First, F>
             && std::is_nothrow_constructible_v<Second, S>)
    : FirstT(std::forward<F>(first)), SecondT(std::forward<S>(second)) { }

    template <typename FirstTuple, typename SecondTuple,
              typename = std::void_t<decltype(std::tuple_size_v<FirstTuple>),
                                     decltype(std::tuple_size_v<SecondTuple>)>>
    constexpr Pair(std::piecewise_construct_t,  FirstTuple &&first_args,
                   SecondTuple &&second_args)
    noexcept(
        noexcept(Pair(
            std::forward<FirstTuple>(first_args),
            std::forward<SecondTuple>(second_args),
            std::make_index_sequence<std::tuple_size_v<FirstTuple>>{ },
            std::make_index_sequence<std::tuple_size_v<SecondTuple>>{ }
        ))
    )
    : Pair{ std::forward<FirstTuple>(first_args),
            std::forward<SecondTuple>(second_args),
            std::make_index_sequence<std::tuple_size_v<FirstTuple>>{ },
            std::make_index_sequence<std::tuple_size_v<SecondTuple>>{ } } { }

    template <typename T, typename U,
              typename =
                  std::enable_if_t<std::is_assignable_v<First, const T&>
                                   && std::is_assignable_v<Second, const U&>>>
    constexpr Pair& operator=(const Pair<T, U> &other)
    noexcept(std::is_nothrow_assignable_v<First, const T&>
             && std::is_nothrow_assignable_v<Second, const U&>) {
        if (this != &other) {
            first() = other.first();
            second() = other.second();
        }

        return *this;
    }

    template <typename T, typename U,
              typename =
                  std::enable_if_t<std::is_assignable_v<First, T&&>
                                   && std::is_assignable_v<Second, U&&>>>
    constexpr Pair& operator=(Pair<T, U> &&other)
    noexcept(std::is_nothrow_assignable_v<First, T&&>
             && std::is_nothrow_assignable_v<Second, U&&>) {
        if (this != &other) {
            first() = std::move(other.first());
            second() = std::move(other.second());
        }

        return *this;
    }

    constexpr inline First& first() noexcept {
        return dynamic_cast<FirstT&>(*this).as_base();
    }

    constexpr inline const First& first() const noexcept {
        return dynamic_cast<const FirstT&>(*this).as_base();
    }

    constexpr inline Second& second() noexcept {
        return dynamic_cast<SecondT&>(*this).as_base();
    }

    constexpr inline const Second& second() const noexcept {
        return dynamic_cast<const SecondT&>(*this).as_base();
    }

    constexpr void swap(Pair &other)
    noexcept(std::is_nothrow_swappable_v<First>
             && std::is_nothrow_swappable_v<Second>)
    {
        using std::swap;

        swap(first(), other.first());
        swap(second(), other.second());
    }

    template <typename T, typename U,
              typename = std::enable_if_t<
                  std::is_swappable_with_v<First, T>
                  && std::is_swappable_with_v<Second, U>
              >>
    constexpr void swap(Pair<T, U> &other)
    noexcept(std::is_nothrow_swappable_with_v<First, T>
             && std::is_nothrow_swappable_with_v<Second, U>)
    {
        using std::swap;

        swap(first(), other.first());
        swap(second(), other.second());
    }

private:
    template <typename FirstTuple, typename SecondTuple,
              std::size_t ...FirstIndices,
              std::size_t ...SecondIndices,
              typename = std::enable_if_t<std::is_constructible_v<
                 First, std::tuple_element_t<FirstIndices, FirstTuple>...
               > && std::is_constructible_v<
                 Second, std::tuple_element_t<SecondIndices, SecondTuple>...
               >>>
    constexpr Pair(FirstTuple &&first_args, SecondTuple &&second_args,
                   std::index_sequence<FirstIndices...>,
                   std::index_sequence<SecondIndices...>)
    noexcept(std::is_nothrow_constructible_v<
                 First, std::tuple_element_t<FirstIndices, FirstTuple>...
             > && std::is_nothrow_constructible_v<
                 Second, std::tuple_element_t<SecondIndices, SecondTuple>...
             >)
    : FirstT(std::get<FirstIndices>(std::forward<FirstTuple>(first_args))...),
      SecondT(std::get<SecondIndices>(
          std::forward<SecondTuple>(second_args)
      )...) { }
};

template <typename CharT, typename Traits, typename First, typename Second>
std::basic_ostream<CharT, Traits>&
operator<<(std::basic_ostream<CharT, Traits> &os,
           const Pair<First, Second> &pair) {
    return os << '(' << pair.first() << ", " << pair.second() << ')';
}

template <typename First1, typename Second1, typename First2, typename Second2>
inline void swap(Pair<First1, Second1> &lhs, Pair<First2, Second2> &rhs)
noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_equality_comparable_v<First1, First2>
              && detail::is_equality_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator==(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(lhs.first() == rhs.first()
                    && lhs.second() == rhs.second())) {
    return lhs.first() == rhs.first() && lhs.second() == rhs.second();
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_equality_comparable_v<First1, First2>
              && detail::is_equality_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator!=(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(lhs == rhs)) {
    return !(lhs == rhs);
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_less_than_comparable_v<First1, First2>
              && detail::is_less_than_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator<(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(lhs.first() == rhs.first()
                    && lhs.second() < rhs.second()
                    && lhs.first() < rhs.first())) {
    if (lhs.first() == rhs.first()) {
        return lhs.second() < rhs.second();
    }

    return lhs.first() < rhs.first();
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_less_than_comparable_v<First1, First2>
              && detail::is_less_than_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator<=(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(rhs < lhs)) {
    return !(rhs < lhs);
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_less_than_comparable_v<First1, First2>
              && detail::is_less_than_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator>(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(rhs < lhs)) {
    return rhs < lhs;
}

template <typename First1, typename Second1, typename First2, typename Second2,
          typename = std::enable_if_t<
              detail::is_less_than_comparable_v<First1, First2>
              && detail::is_less_than_comparable_v<Second1, Second2>
          >>
constexpr inline bool operator>=(
    const Pair<First1, Second1> &lhs, const Pair<First2, Second2> &rhs
) noexcept(noexcept(lhs < rhs)) {
    return !(lhs < rhs);
}

template <typename First, typename Second>
constexpr gregjm::Pair<detail::UnwrapDecayT<First>,
                       detail::UnwrapDecayT<Second>>
make_pair(First &&first, Second &&second) {
    using FirstT = detail::UnwrapDecayT<First>;
    using SecondT = detail::UnwrapDecayT<Second>;
    using PairT = gregjm::Pair<FirstT, SecondT>;

    return PairT{ std::forward<First>(first), std::forward<Second>(second) };
}

} // namespace gregjm

#endif
