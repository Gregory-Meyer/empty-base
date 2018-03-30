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
: private std::conditional_t<detail::is_inheritable_v<First>,
                             First, detail::Wrapper<First>>,
  private std::conditional_t<detail::is_inheritable_v<Second>,
                             detail::Alias<Second>,
                             detail::AltWrapper<Second>> {
private:
    using FirstT = std::conditional_t<detail::is_inheritable_v<First>,
                                      First, detail::Wrapper<First>>;
    using SecondT = std::conditional_t<detail::is_inheritable_v<Second>,
                                       detail::Alias<Second>,
                                       detail::AltWrapper<Second>>;

public:
    using first_type = First;
    using second_type = Second;

    constexpr Pair()
    noexcept(std::is_nothrow_default_constructible_v<First>
             && std::is_nothrow_default_constructible_v<Second>) = default;

    template <typename T, typename U,
              typename =
                  std::enable_if_t<std::is_constructible_v<First, T>
                                   && std::is_constructible_v<Second, U>>>
    explicit constexpr Pair(const Pair<T, U> &other)
    noexcept(std::is_nothrow_constructible_v<First, const T&>
             && std::is_nothrow_constructible_v<Second, const U&>)
    : FirstT{ other.first() }, SecondT{ other.second() } { }

    template <typename T, typename U,
              typename =
                  std::enable_if_t<std::is_constructible_v<First, T>
                                   && std::is_constructible_v<Second, U>>>
    explicit constexpr Pair(Pair<T, U> &&other)
    noexcept(std::is_nothrow_constructible_v<First, T&&>
             && std::is_nothrow_constructible_v<Second, U&&>)
    : FirstT{ std::move(other.first()) },
      SecondT{ std::move(other.second()) } { }

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

    constexpr inline First& first() noexcept {
        if constexpr (!detail::is_inheritable_v<First>) {
            return dynamic_cast<detail::Wrapper<First>&>(*this).data;
        } else {
            return dynamic_cast<First&>(*this);
        }
    }

    constexpr inline const First& first() const noexcept {
        if constexpr (!detail::is_inheritable_v<First>) {
            return dynamic_cast<const detail::Wrapper<First>&>(*this).data;
        } else {
            return dynamic_cast<const First&>(*this);
        }
    }

    constexpr inline Second& second() noexcept {
        if constexpr (!detail::is_inheritable_v<Second>) {
            return dynamic_cast<detail::AltWrapper<Second>&>(*this).data;
        } else {
            return dynamic_cast<detail::Alias<Second>&>(*this).as_base();
        }
    }

    constexpr inline const Second& second() const noexcept {
        if constexpr (!detail::is_inheritable_v<Second>) {
            return dynamic_cast<const detail::AltWrapper<Second>&>(*this).data;
        } else {
            return dynamic_cast<const detail::Alias<Second>&>(*this).as_base();
        }
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

} // namespace gregjm

#endif
