//
// Created by olber on 8/10/2022.
//

#pragma once
#include <cstdlib>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <iostream>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <vector>
#include <numeric>

#include "Vector_X.h"
#include "LinearAlgebraTypeTraits.h"

namespace linear_algebra_core
{
    template <size_t N, IsArithmetic value_type>
    class Point_X
    {
    private:
        std::array<value_type, N> m_values;

    public:
        Point_X() : m_values{} { }

        template <DoesNotNarrowlyConvertTo<value_type>... InitialValues>
        explicit Point_X(InitialValues... initialValues)
        {
            static_assert(N == sizeof...(InitialValues), "Incorrect number of parameters given to constructor");
            m_values = std::array<value_type, N>{initialValues...};
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        constexpr Point_X(std::initializer_list<other_type> initialValues)
        {
            if(initialValues.size() != N) {
                throw std::invalid_argument("Point_X constructor expected a initializer_list of size " + std::to_string(N) + ", but got size " + std::to_string(initialValues.size()));
            }
            std::copy_n(std::begin(initialValues), N, begin());
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        explicit Point_X(const std::array<other_type, N>& initialValues) : m_values{initialValues} { }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        explicit Point_X(const std::array<other_type, N>&& initialValues) : m_values{initialValues} { }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        explicit Point_X(const other_type (&initialValues)[N]) {
            std::copy_n(std::begin(initialValues), N, std::begin(m_values));
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        explicit Point_X(const std::vector<other_type>& initialValues) {
            if(initialValues.size() != N) {
                throw std::out_of_range("Point_X constructor expected a std::vector of size " + std::to_string(N) + ", but got size " + std::to_string(initialValues.size()));
            }
            std::copy_n(std::begin(initialValues), N, std::begin(m_values));
        }

        ~Point_X() = default;

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X(const Point_X<N, other_type>& other)
        {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), N, begin());
            }
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X(Point_X<N, other_type>&& other) noexcept : m_values{std::move(other.m_values)} { }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X<N, value_type>& operator=(const Point_X<N, other_type>& other)
        {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), N, begin());
            }
            return *this;
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X<N, value_type>& operator=(Point_X<N, other_type>&& other) noexcept
        {
            m_values = std::move(other.m_values);
            return *this;
        }

        // iterator exposure
        auto begin()   { return std::begin(m_values);   }
        auto end()     { return std::end(m_values);     }
        auto rbegin()  { return std::rbegin(m_values);  }
        auto rend()    { return std::rend(m_values);    }
        [[nodiscard]] auto cbegin()  const { return std::cbegin(m_values);  }
        [[nodiscard]] auto cend()    const { return std::cend(m_values);    }
        [[nodiscard]] auto crbegin() const { return std::crbegin(m_values); }
        [[nodiscard]] auto crend()   const { return std::crend(m_values);   }

        /*!
         * @param index index of the value to return
         * @return the value at the given \p index
         */
        [[nodiscard]] inline value_type operator[](size_t index) const { return m_values[index]; }
        /*!
         * @param index index of the value to return
         * @return the value at the given \p index
         */
        [[nodiscard]] inline value_type& operator[](size_t index) { return m_values[index]; }

        /*!
         * Throws an exception if index is out of bounds
         * @param index index of the value to return
         * @return the value at the given \p index
         */
        [[nodiscard]] inline value_type at(size_t index) const { return m_values.at(index); }
        /*!
         * Throws an exception if index is out of bounds
         * @param index index of the value to return
         * @return the value at the given \p index
         */
        [[nodiscard]] inline value_type& at(size_t index) { return m_values.at(index); }

        /*!
         * Compile-time element access
         * @tparam Index index of the desired value
         * @return a copy of the value at the given \p Index
         */
        template<size_t Index>
        [[nodiscard]] constexpr value_type getValue() const requires (Index < N)
        {
            return std::get<Index>(m_values);
        }
        /*!
         * Compile-time element access
         * @tparam Index index of the desired value
         * @return a reference to the value at the given \p Index
         */
        template<size_t Index>
        [[nodiscard]] constexpr value_type& getValue() requires (Index < N)
        {
            return std::get<Index>(m_values);
        }

        /*!
         * Scales this Point by \p scalar
         * @tparam T The type of \p scalar. Must be an arithmetic type.
         * @param scalar Value to scale the point by
         * @return The scaled point
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Point_X<N, value_type> operator*(T scalar) const
        {
            Point_X<N, value_type> result;
            std::transform(cbegin(), cend(), result.begin(), [scalar](auto value) { return value * scalar; });
            return result;
        }

        /*!
         * Scales \p rhs by \p scalar
         * @tparam T Type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale \p rhs by
         * @param rhs Point to be scaled
         * @return the scaled point
         */
        template<IsArithmetic result_value_type, DoesNotNarrowlyConvertTo<result_value_type> T>
        [[nodiscard]] friend inline
        Point_X<N, result_value_type> operator*(T scalar, const Point_X<N, result_value_type>& rhs)
        {
            return rhs * scalar;
        }

        /*!
         * Scale this point by \p scalar
         * @tparam T Type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale this Point by
         * @return a reference to this Point
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Point_X<N, value_type>& operator*=(T scalar)
        {
            std::transform(cbegin(), cend(), begin(), [scalar](auto value) { return value * scalar; });
            return *this;
        }

        /*!
         * Scale this point by 1/\p scalar
         * @tparam T Type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale this point by
         * @return the scaled point
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Point_X<N, value_type> operator/(T scalar) const
        {
            if(scalar == 0.0)
            {
                throw std::logic_error("Cannot divide a point by 0");
            }
            Point_X<N, value_type> result;
            std::transform(cbegin(), cend(), result.begin(), [scalar](auto value) { return value / scalar; });
            return result;
        }

        /*!
         * Scale this point by 1/\p scalar
         * @tparam T Type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale this vector by
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Point_X<N, value_type>& operator/=(T scalar)
        {
            if(scalar == 0.0)
            {
                throw std::logic_error("Cannot divide a point by 0");
            }
            std::transform(cbegin(), cend(), begin(), [scalar](auto value) { return value / scalar; });
            return *this;
        }

        /*!
         * Point-Vector addition
         * @param rhs Vector used to translate this point
         * @return The translated point
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Point_X<N, value_type> operator+(const Vector_X<N, other_type>& rhs) const
        {
            Point_X<N, value_type> result;
            std::transform(cbegin(), cend(), rhs.cbegin(), result.begin(), [](auto left, auto right) { return left + right; });
            return result;
        }

        /*!
         * In-place Point-Vector addition
         * @param rhs Vector used to translate this point
         * @return a reference to this point
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X<N, value_type>& operator+=(const Vector_X<N, other_type>& rhs)
        {
            std::transform(cbegin(), cend(), rhs.cbegin(), begin(), [](auto left, auto right) { return left + right; });
            return *this;
        }

        /*!
         * Unary negation operator
         * @return a negated version of this point
         */
        [[nodiscard]] inline Point_X<N, value_type> operator-() const
        {
            return (*this) * static_cast<value_type>(-1.0);
        }

        /*!
         * Point-Point subtraction
         * @param rhs The point to subtract from this point
         * @return The result of the subtraction
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Vector_X<N, value_type> operator-(const Point_X<N, other_type>& rhs) const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), rhs.cbegin(), result.begin(), [](auto left, auto right) { return left - right; });
            return result;
        }

        /*!
         * Point-Vector subtraction. Equivalent to Point + -Vector
         * @param rhs Vector used to translate the point by
         * @return The result of the subtraction
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Point_X<N, value_type> operator-(const Vector_X<N, other_type>& rhs) const
        {
            Point_X<N, value_type> result;
            std::transform(cbegin(), cend(), rhs.cbegin(), result.begin(), [](auto left, auto right) { return left - right; });
            return result;
        }

        /*!
         * In-place Point-Vector subtraction. Equivalent to Point += -Vector
         * @param rhs Vector used to translate this point by
         * @return a reference to this point
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Point_X<N, value_type>& operator-=(const Vector_X<N, other_type>& rhs)
        {
            std::transform(cbegin(), cend(), rhs.cbegin(), begin(), [](auto left, auto right) { return left - right; });
            return *this;
        }

        /*!
         * @param rhs point to check equality against
         * @return true if \p rhs and this point are equal. false otherwise
         */
        template<IsArithmetic other_type>
        [[nodiscard]] bool operator==(const Point_X<N, other_type>& rhs) const
        {
            for(size_t i = 0; i < N; i++) {
                if (m_values[i] != rhs[i]) {
                    return false;
                }
            }
            return true;
        }

        /*!
         * @param rhs point to check inequality against
         * @return true if \p rhs and this point are not equal. false otherwise
         */
        template<IsArithmetic other_type>
        [[nodiscard]] inline bool operator!=(const Point_X<N, other_type>& rhs) const
        {
            return !(*this == rhs);
        }

        /*!
         * @return the string representation of this point.
         */
        [[nodiscard]] std::string to_string() const
        {
            std::string output = "{ ";
            for(int i = 0; i < (N - 1); i++) {
                output += std::to_string(m_values[i]) + ", ";
            }
            if constexpr (N > 0) {
                output += std::to_string(m_values[N - 1]);
            }
            output += " }";
            return output;
        }

        /*!
         * ostream insertion operator
         * @param out ostream to insert \p rhs into
         * @param rhs point to be insert into \p out
         * @return a reference to \p out
         */
        friend std::ostream& operator<<(std::ostream& out, const Point_X<N, value_type>& rhs)
        {
            out << rhs.to_string();
            return out;
        }

        /*!
         * Square each element of this point
         * @return a reference to this point
         */
        Point_X<N, value_type>& square()
        {
            std::transform(cbegin(), cend(), begin(), [](auto value) { return value * value; });
            return *this;
        }

        /*!
         * @return a squared copy of this point
         */
        [[nodiscard]] [[maybe_unused]] inline Point_X<N, value_type> getSquared() const
        {
            Point_X<N, value_type> result {*this};
            return result.square();
        }

        /*!
         * @return the sum of each element of this point
         */
        [[nodiscard]] [[maybe_unused]] inline value_type sumElements() const
        {
            return std::accumulate(cbegin(), cend(), static_cast<value_type>(0));
        }

        /*!
         * @return the maximum element in the vector
         */
        [[nodiscard]] inline value_type getMaxValue() const
        {
            return *std::max_element(cbegin(), cend());
        }

        /*!
         * @return the minimum element in the vector
         */
        [[nodiscard]] inline value_type getMinValue() const
        {
            return *std::min_element(cbegin(), cend());
        }

        /*!
         * Passes each element of the point to /p func and assign the element the resulting value
         * @param func a unary function that is given the current value of the element as a parameter
         * @return this point, modified.
         */
        Point_X<N, value_type>& for_each(const std::function<value_type(const value_type &)>& func)
        {
            for(auto& val : m_values) {
                val = func(val);
            }
            return *this;
        }

        /*!
         * Passes each element of this point and the corresponding element of /p other to /p func and assigns this element the resulting value
         * @param other another vector to operate on
         * @param func the function that is given elements of this point and /p other
         * @return this point, modified.
         */
        Point_X<N, value_type>& for_each(const Point_X<N, value_type>& other, const std::function<value_type(const value_type &, const value_type&)>& func)
        {
            for(auto this_it = begin(), other_it = other.cbegin(); this_it != end(); this_it++, other_it++)
            {
                *this_it = func(*this_it, *other_it);
            }
            return *this;
        }

        /*!
         * Returns a point of dimension \p M. If \p M is smaller than this point's size, any extra values from this point
         * will be truncated. If \p M is large than this point's size, all extra values will be filled with 0.0.
         * @tparam M The size of the Point to return
         * @return The new point
         */
        template<size_t M>
        [[maybe_unused]]
        Point_X<M, value_type> getAsDimension() const
        {
            if constexpr (M == N) {
                return *this;
            } else if constexpr (M < N) {
                return Point_X<M, value_type>(cbegin(), cbegin() + M);
            } else {
                Point_X<M, value_type> result;
                std::copy_n(cbegin(), N, result.begin());
                return result;
            }
        }

        /*!
         * @return A vector filled with the values in this point
         */
        [[nodiscard]] [[maybe_unused]] inline Vector_X<N, value_type> to_Vector() const
        {
            return Vector_X<N, value_type>{m_values};
        }

        /*!
         * Interpolates between \p a and \p b, by the factor \p t. A \p t value of 0.0 will return the point \p a, and a
         * \p t value of 1.0 will return the point \p b
         * @param a The first point to interpolate between
         * @param b The second point to interpolate between
         * @param t the interpolation value. must be between 0.0 and 1.0
         * @return The interpolated point
         */
        template<IsArithmetic a_type, DoesNotNarrowlyConvertTo<a_type> b_type, DoesNotNarrowlyConvertTo<a_type> T>
        [[nodiscard]] [[maybe_unused]] static Point_X<N, a_type> linear_interpolation(const Point_X<N, a_type>& a, const Point_X<N, b_type>& b, T t)
        {
            if(t > 1.0 || t < 0.0)
            {
                throw std::invalid_argument("t must be in the range of [0.0, 1.0]. Given value was: " + std::to_string(t));
            }
            return (a * t) + (b * (1.0 - t));
        }
    };
}