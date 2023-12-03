//
// Created by Matt on 9/18/2023.
//
#pragma once

#include <type_traits>

namespace linear_algebra_core {
    namespace detail {
        template<typename From, typename To, typename = void>
        struct is_narrowing_conversion_impl : std::true_type {
        };

        template<typename From, typename To>
        struct is_narrowing_conversion_impl<From, To, std::void_t<decltype(To{(std::declval<From>())})>> : std::false_type {
        };

        template<typename From, typename To>
        struct is_narrowing_conversion : detail::is_narrowing_conversion_impl<From, To> {
        };

        template< class From, class To >
        inline constexpr bool is_narrowing_conversion_v = is_narrowing_conversion<From, To>::value;

        template< class From, class To >
        inline constexpr bool is_not_narrowing_conversion_v = !is_narrowing_conversion_v<From, To>;
    }  // namespace detail

    template<typename T>
    concept IsArithmetic = std::is_arithmetic_v<T>;

    template<typename T>
    concept IsFloatingPoint = std::is_floating_point_v<T>;

    template<typename From, typename To>
    concept DoesNotNarrowlyConvertTo = IsArithmetic<To> && IsArithmetic<From> && detail::is_not_narrowing_conversion_v<From, To>;
}