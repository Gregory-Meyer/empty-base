#ifndef GREGJM_PAIR_DETAIL_HPP
#define GREGJM_PAIR_DETAIL_HPP

#include <type_traits>
#include <utility> // std::forward

namespace gregjm {
namespace detail {

template <typename T>
struct Wrapper {
    T data;

    constexpr Wrapper()
    noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template <typename ...Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit Wrapper(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : data(std::forward<Args>(args)...) { }
};

template <typename T>
struct AltWrapper {
    T data;

    constexpr AltWrapper()
    noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template <typename ...Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit AltWrapper(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : data(std::forward<Args>(args)...) { }
};

template <typename T>
struct Alias : private T {
    constexpr Alias()
    noexcept(std::is_nothrow_default_constructible_v<T>) = default;

    template <typename ...Args,
              typename = std::enable_if_t<std::is_constructible_v<T, Args...>>>
    constexpr explicit Alias(Args &&...args)
    noexcept(std::is_nothrow_constructible_v<T, Args...>)
    : T(std::forward<Args>(args)...) { }

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

} // namespace detail
} // namespace gregjm

#endif
