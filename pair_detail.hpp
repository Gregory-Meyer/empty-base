#ifndef GREGJM_PAIR_DETAIL_HPP
#define GREGJM_PAIR_DETAIL_HPP

#include <type_traits>
#include <utility> // std::forward

namespace gregjm {
namespace detail {

template <typename T, std::size_t I = 0>
struct Wrapper {
    T data;

    constexpr Wrapper()
    noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template <typename ...Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit Wrapper(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : data(std::forward<Args>(args)...) { }

    template <typename U,
              typename = std::enable_if_t<
                  std::is_constructible_v<T, std::initializer_list<U>>
              >>
    constexpr explicit Wrapper(const std::initializer_list<U> init)
    noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>>)
    : data{ init } { }

    constexpr inline T& as_base() noexcept {
        return data;
    }

    constexpr inline const T& as_base() const noexcept {
        return data;
    }
};

template <typename T, std::size_t I = 0>
struct Alias : private T {
    constexpr Alias()
    noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template <typename ...Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit Alias(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : T(std::forward<Args>(args)...) { }

    template <typename U,
              typename = std::enable_if_t<
                  std::is_constructible_v<T, std::initializer_list<U>>
              >>
    constexpr explicit Alias(const std::initializer_list<U> init)
    noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>>)
    : T{ init } { }

    constexpr inline T& as_base() noexcept {
        return dynamic_cast<T&>(*this);
    }

    constexpr inline const T& as_base() const noexcept {
        return dynamic_cast<const T&>(*this);
    }
};

template <typename T>
struct IsInheritable
: std::conditional_t<std::is_class_v<T> && !std::is_final_v<T>,
                     std::true_type, std::false_type> { };

template <typename T>
static constexpr inline bool is_inheritable_v = IsInheritable<T>::value;

template <typename T, typename U = T, typename = void>
struct IsEqualityComparable : std::false_type { };

template <typename T, typename U>
struct IsEqualityComparable<
    T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>()),
                      decltype(std::declval<T>() != std::declval<U>()),
                      decltype(std::declval<U>() == std::declval<T>()),
                      decltype(std::declval<U>() != std::declval<T>())>>
: std::true_type { };

template <typename T, typename U = T>
static constexpr inline bool is_equality_comparable_v =
    IsEqualityComparable<T, U>::value;

template <typename T, typename U = T, typename = void>
struct IsLessThanComparable : std::false_type { };

template <typename T, typename U>
struct IsLessThanComparable<
    T, U, std::void_t<decltype(std::declval<T>() == std::declval<U>()),
                      decltype(std::declval<T>() != std::declval<U>()),
                      decltype(std::declval<T>() < std::declval<U>()),
                      decltype(std::declval<T>() <= std::declval<U>()),
                      decltype(std::declval<T>() > std::declval<U>()),
                      decltype(std::declval<T>() >= std::declval<U>()),
                      decltype(std::declval<U>() == std::declval<T>()),
                      decltype(std::declval<U>() != std::declval<T>()),
                      decltype(std::declval<U>() < std::declval<T>()),
                      decltype(std::declval<U>() <= std::declval<T>()),
                      decltype(std::declval<U>() > std::declval<T>()),
                      decltype(std::declval<U>() >= std::declval<T>())>>
: std::true_type { };

template <typename T, typename U = T>
static constexpr inline bool is_less_than_comparable_v =
    IsLessThanComparable<T, U>::value;

template <typename T, std::size_t I = 0>
struct WrapIfNotInheritable {
    using TypeT = std::conditional_t<is_inheritable_v<T>, Alias<T, I>,
                                     Wrapper<T, I>>;
};

template <typename T, std::size_t I = 0>
using WrapIfNotInheritableT = typename WrapIfNotInheritable<T, I>::TypeT;

template <typename T>
struct Unwrap {
    using TypeT = T;
};

template <typename T>
struct Unwrap<std::reference_wrapper<T>> {
    using TypeT = T;
};

template <typename T>
using UnwrapT = typename Unwrap<T>::TypeT;

template <typename T>
struct UnwrapDecay {
    using TypeT = UnwrapT<std::decay_t<T>>;
};

template <typename T>
using UnwrapDecayT = typename UnwrapDecay<T>::TypeT;

} // namespace detail
} // namespace gregjm

#endif
