//
// Created by olber on 8/10/2022.
//

#pragma once
#include <cstdlib>
#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <iosfwd>
#include <stdexcept>
#include <iterator>
#include <type_traits>
#include <vector>
#include <numeric>

#include "LinearAlgebraTypeTraits.h"

namespace linear_algebra_core
{

    template <size_t N, IsArithmetic value_type>
    class Vector_X
    {
    private:
        std::array<value_type, N> m_values{};

    public:
        Vector_X() = default;

        template <DoesNotNarrowlyConvertTo<value_type>... InitialValues>
        explicit constexpr Vector_X(InitialValues... initialValues)
        {
            static_assert(N == sizeof...(InitialValues), "Incorrect number of parameters given to constructor");
            m_values = std::array<value_type, N>{initialValues...};
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        constexpr Vector_X(std::initializer_list<T> initialValues)
        {
            if (initialValues.size() != N) {
                throw std::invalid_argument("Vector_X constructor expected a initializer_list of size " + std::to_string(N) + ", but got size " + std::to_string(initialValues.size()));
            }
            std::copy_n(std::cbegin(initialValues), N, begin());
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        explicit constexpr Vector_X(const std::array<T, N>& initialValues) : m_values{initialValues} { }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        explicit constexpr Vector_X(const std::array<T, N>&& initialValues) : m_values{initialValues} { }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        explicit constexpr Vector_X(const T (&initialValues)[N]) {
            std::copy_n(std::begin(initialValues), N, begin());
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        explicit Vector_X(const std::vector<T>& initialValues) {
            if(initialValues.size() != N) {
                throw std::invalid_argument("Vector_X constructor expected a std::vector of size " + std::to_string(N) + ", but got size " + std::to_string(initialValues.size()));
            }
            std::copy_n(std::begin(initialValues), N, begin());
        }
        template<typename Iterator>
        Vector_X(Iterator Begin, Iterator End)
        {
            static_assert(detail::is_not_narrowing_conversion_v<decltype(*Begin), value_type>, "the given underlying type of the iterator must not be a narrowing conversion to value_type");
            static_assert(std::is_arithmetic_v<decltype(*Begin)>, "The underlying type of the given iterator must be an arithmetic type");
            if(std::distance(Begin, End) != N) {
                throw std::invalid_argument("Vector_X constructor expected an iterable span of size " + std::to_string(N) + ", but got size " + std::to_string(std::distance(Begin, End)));
            }
            std::copy_n(Begin, N, begin());
        }

        ~Vector_X() = default;

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X(const Vector_X<N, other_type>& other)
        {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), N, begin());
            }
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X(Vector_X<N, other_type>&& other) noexcept : m_values{std::move(other.m_values)} { }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X<N, value_type>& operator=(const Vector_X<N, other_type>& other)
        {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), N, begin());
            }
            return *this;
        }

        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X<N, value_type>& operator=(Vector_X<N, other_type>&& other) noexcept
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
         * The dot product of two vectors. Vectors must be the same size
         * @param rhs The other vector used to calculate the dot product
         * @return the dot product of this vector and \p rhs
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] [[maybe_unused]] inline value_type dot(const Vector_X<N, other_type>& rhs) const
        {
            return (*this) * rhs;
        }

        /*!
         * The cross product of two vectors. Both vectors must be of size 3.
         * @param rhs the other vector used to calculate the cross product
         * @return The cross product of this vector and \p rhs
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Vector_X<N, value_type> cross(const Vector_X<N, other_type>& rhs) const
        {
            static_assert(N == 3, "cross product can only be computed on 3 dimensional vectors");
            return {
                m_values[1] * rhs[2] - m_values[2] * rhs[1],
                m_values[2] * rhs[0] - m_values[0] * rhs[2],
                m_values[0] * rhs[1] - m_values[1] * rhs[0]
            };
        }

        /*!
         * Fill the vector with the given \p value
         * @param value values used to fill the vector
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Vector_X<N, value_type>& fill(T value)
        {
            std::fill(begin(), end(), value);
            return (*this);
        }

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
         * Calculate the dot product of two vectors. Vectors must be the same size.
         * @param rhs the other vector used to calculate the dot product
         * @return The dot product of this vector and \p rhs
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] value_type operator*(const Vector_X<N, other_type>& rhs) const
        {
            value_type result = 0;
            for(int i = 0; i < N; i++) {
                result += m_values[i] * rhs[i];
            }
            return result;
        }

        /*!
         * Return a vector that's the result of scaling this vector by \p scalar
         * @tparam T The type of \p scalar. Must be an Arithmetic type.
         * @param scalar value to scale the Vector by
         * @return The scaled vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Vector_X<N, value_type> operator*(T scalar) const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), result.begin(), [scalar](auto value) { return value * scalar; });
            return result;
        }

        /*!
         * Scale \p rhs by \p scalar
         * @tparam T The type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale \p rhs by.
         * @param rhs vector to be scaled.
         * @return the scaled vector
         */
        template<IsArithmetic result_value_type, DoesNotNarrowlyConvertTo<result_value_type> T>
        [[nodiscard]] friend inline
        Vector_X<N, result_value_type> operator*(T scalar, const Vector_X<N, result_value_type>& rhs)
        {
            return rhs * scalar;
        }

        /*!
         * Scale this vector by the given \p scalar
         * @tparam T The type of the \p scalar. Must be an arithmetic type.
         * @param scalar The value to scale this vector by.
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Vector_X<N, value_type>& operator*=(T scalar)
        {
            std::for_each(begin(), end(), [scalar](auto& value) { value *= scalar; });
            return *this;
        }

        /*!
         * Returns a vector containing result of scaling this vector by 1/\p scalar
         * @tparam T The type of \p scalar. Must be an arithmetic type
         * @param scalar The value to scale this vector by. Cannot be 0
         * @return the scaled vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Vector_X<N, value_type> operator/(T scalar) const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), result.begin(), [scalar](auto value) { return value / scalar; });
            return result;
        }

        /*!
         * Scale this vector by 1/\p scalar.
         * @tparam T The type of the \p scalar. Must be an arithmetic type.
         * @param scalar The value to scale this vector by. Cannot be 0
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Vector_X<N, value_type>& operator/=(T scalar)
        {
            std::for_each(begin(), end(), [scalar](auto& value) { value /= scalar; });
            return *this;
        }

        /*!
         * Returns a vector that is the result of adding this vector to \p rhs. Vectors must be the same size.
         * @param rhs Vector to add this vector to.
         * @return The result of the vector addition.
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Vector_X<N, value_type> operator+(const Vector_X<N, other_type>& rhs) const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), rhs.cbegin(), result.begin(), [](auto left, auto right) { return left + right; });
            return result;
        }

        /*!
         * Adds \rhs to this vector.
         * @param rhs Vector to add to this vector
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X<N, value_type>& operator+=(const Vector_X<N, other_type>& rhs)
        {
            std::transform(cbegin(), cend(), rhs.cbegin(), begin(), [](auto left, auto right) { return left + right; });
            return *this;
        }

        /*!
         * Unary negation operator. Multiples this vector by -1.0.
         * @return a reference to this vector
         */
        [[nodiscard]] inline Vector_X<N, value_type> operator-() const
        {
            std::for_each(begin(), end(), [](auto& value) { value *= -1; });
            return *this;
        }

        /*!
         * Returns the result of subtracting \p rhs from this vector.
         * @param rhs The vector to subtract
         * @return The result of the vector subtraction.
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Vector_X<N, value_type> operator-(const Vector_X<N, other_type>& rhs) const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), rhs.cbegin(), result.begin(), [](auto left, auto right) { return left - right; });
            return result;
        }

        /*!
         * Subtract \p rhs from this vector
         * @param rhs The vector to subtract
         * @return a reference to this vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Vector_X<N, value_type>& operator-=(const Vector_X<N, other_type>& rhs)
        {
            std::transform(cbegin(), cend(), rhs.cbegin(), begin(), [](auto left, auto right) { return left - right; });
            return *this;
        }

        /*!
         * Checks if each element of this vector is equal to the corresponding element of \p rhs
         * @param rhs The vector to compare against
         * @return true if the two vectors are equal, false otherwise.
         */
        template<IsArithmetic other_type>
        [[nodiscard]] bool operator==(const Vector_X<N, other_type>& rhs) const
        {
            return std::equal(cbegin(), cend(), rhs.cbegin(), rhs.cend());
        }

        /*!
         * Checks if any element of this vector differ from corresponding element of \p rhs
         * @param rhs The vector to compare this to
         * @return true if the two vectors are different, false otherwise.
         */
        template<IsArithmetic other_type>
        [[nodiscard]] inline bool operator!=(const Vector_X<N, other_type>& rhs) const
        {
            return !std::equal(cbegin(), cend(), rhs.cbegin(), rhs.cend());
        }

        /*!
         * @return the summation of the square of each value of this vector
         */
        [[nodiscard]] inline value_type getMagnitudeSquared() const
        {
            value_type result = 0;
            for(auto value : m_values) {
                result += value * value;
            }
            return result;
        }

        /*!
         * @return The magnitude of this vector
         */
        [[nodiscard]] inline value_type getMagnitude() const
        {
            return static_cast<value_type>(std::sqrt(getMagnitudeSquared()));
        }

        /*!
         * Normalizes this vector by dividing each element by the magnitude of the vector
         * @return a reference to this vector
         */
        inline Vector_X<N, value_type>& normalize()
        {
            value_type inverse_magnitude = 1 / getMagnitude();
            (*this) *= inverse_magnitude;
            return *this;
        }

        /*!
         * @return a normalized copy of this vector
         */
        [[nodiscard]] inline Vector_X<N, value_type> getUnitVector() const
        {
            value_type inverse_magnitude = 1 / getMagnitude();
            return (*this) * inverse_magnitude;
        }

        [[nodiscard]] inline Vector_X<N, value_type> getInverse() const
        {
            Vector_X<N, value_type> result;
            std::transform(cbegin(), cend(), result.begin(), [](auto value) { return 1.0 / value; });
            return result;
        }

        /*!
         * Square each element of this vector
         * @return a reference to this vector
         */
        Vector_X<N, value_type>& square()
        {
            std::for_each(begin(), end(), [](auto& value) { value *= value; });
            return (*this);
        }

        /*!
         * @return a squared copy of this vector
         */
        [[nodiscard]] [[maybe_unused]] inline Vector_X<N, value_type> getSquared() const
        {
            Vector_X<N, value_type> result {*this};
            return result.square();
        }

        /*!
         * @return the summation of each element of this vector
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
         * Passes each element of the vector to /p func and assign the element the resulting value
         * @param func a unary function that is given the current value of the element as a parameter
         * @return this vector, modified.
         */
        inline Vector_X<N, value_type>& for_each(const std::function<value_type(const value_type &)>& func)
        {
            for(auto& val : m_values) {
                val = func(val);
            }
            return *this;
        }

        /*!
         * Passes each element of this vector and the corresponding element of /p other to /p func and assigns this element the resulting value
         * @param other another vector to operate on
         * @param func the function that is given elements of this vector and /p other
         * @return this vector, modified.
         */
        inline Vector_X<N, value_type>& for_each(const Vector_X<N, value_type>& other, const std::function<value_type(const value_type&, const value_type&)>& func)
        {
            auto other_it = other.cbegin();
            for(auto this_it = begin(); this_it != end(); this_it++, other_it++)
            {
                *this_it = func(*this_it, *other_it);
            }
            return *this;
        }

        /*!
         * If /p new_type is the same as value_type, this function is a noop, otherwise returns
         * a new vector with the values equal to the result of static casting the values in this vector.
         * @tparam new_type The underlying type of the new vector
         * @return the new vector
         */
        template<IsArithmetic new_type>
        Vector_X<N, new_type> As() const {
            if constexpr (std::is_same_v<value_type, new_type>) {
                return *this;
            } else {
                Vector_X<N, new_type> result;
                result.for_each([](auto value) { return static_cast<new_type>(value); });
                return result;
            }
        }

        /*!
         * Returns a vector of dimension \p M. If M is smaller than this vector's size, any extra values from this vector
         * will be truncated. If M is large than this vectors size, all extra values will be filled with 0.0.
         * @tparam M The size of the Vector to return
         * @return The new vector
         */
        template<size_t M>
        [[maybe_unused]]
        Vector_X<M, value_type> getAsDimension() const
        {
            if constexpr (M == N) {
                return *this;
            } else if constexpr (M < N) {
                return Vector_X<M, value_type>(cbegin(), cbegin() + M);
            } else {
                Vector_X<M, value_type> result;
                std::copy_n(cbegin(), N, result.begin());
                return result;
            }
        }

        /*!
         * Projects this vector onto \p other.
         * @param other vector for this to be projected onto.
         * @return The projected vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] [[maybe_unused]] Vector_X<N, value_type> projectOnto(const Vector_X<N, other_type>& other) const
        {
            Vector_X<N, value_type> unit_other = other.getUnitVector();
            return unit_other * ((*this) * unit_other);
        }

        /*!
         * Returns the angle between this vector and \p other in radians.
         * @param other Vector to get the angle between
         * @return the angle between this vector and \p other, in radians.
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] [[maybe_unused]] inline value_type getAngleBetween(const Vector_X<N, other_type>& other) const
        {
            return acos(((*this) * other) / (getMagnitude() * other.getMagnitude()));
        }

        /*!
         * Checks the orthogonality of this vector and \p other.
         * @param other vector to check orthogonality with
         * @return true if the vectors are orthogonal, false otherwise.
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] inline bool isOrthogonalTo(const Vector_X<N, other_type>& other) const
        {
            return ((*this) * other) == 0;
        }

        /*!
         * Interpolates between this vector and \p other, by the factor \p t. A \p t value of 0.0 will leave this vector
         * unchanged, and a \p t value of 1.0 will set this vector to \p other
         * @param other The other vector to interpolate this vector with
         * @param t the interpolation value. must be between 0.0 and 1.0
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type, DoesNotNarrowlyConvertTo<value_type> T>
        [[maybe_unused]]
        inline void interpolateWith(const Vector_X<N, other_type>& other, T t)
        {
            if(t > 1.0 || t < 0.0)
            {
                throw std::invalid_argument("t must be in the range of [0.0, 1.0]. Given value was: " + std::to_string(t));
            }
            (*this) *= (1.0 - t);
            (*this) += (other * t);
        }

        /*!
         * Interpolates between this vector and \p other, by the factor \p t. A \p t value of 0.0 will return this vector,
         * and a \p t value of 1.0 will return \p other.
         * @param other The other vector to interpolate this vector with
         * @param t the interpolation value. must be between 0.0 and 1.0
         * @return the interpolated vector
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type, IsArithmetic T>
        [[nodiscard]] [[maybe_unused]] Vector_X<N, value_type> getInterpolatedVector(const Vector_X<N, other_type>& other, T t) const
        {
            return Vector_X<N, value_type>::linear_interpolation(*this, other, t);
        }

        /*!
         * @return the string representation of this vector.
         */
        [[nodiscard]] std::string to_string() const
        {
            std::string output = "{ ";
            for(int i = 0; i < (N - 1); i++) {
                output += std::to_string(m_values[i]) + ", ";
            }
            if constexpr (N > 0) {
                output += std::to_string(m_values[N - 1]) + " ";
            }
            output += "}";
            return output;
        }

        /*!
         * Adds a string representation of \p rhs to \p out
         * @param out the ostream to add \p rhs to
         * @param rhs the vector to add to \p out
         * @return a reference to \p out
         */
        friend std::ostream& operator<<(std::ostream& out, const Vector_X<N, value_type>& rhs)
        {
            out << rhs.to_string();
            return out;
        }

        /*!
         * Returns the triple scalar product of \p a, \p b, and \p c, as calculated by a.dot(b.cross(c));
         * @param a The \p a vector in the above formula
         * @param b the \p b vector in the above formula
         * @param c the \p c vector in the above formula
         * @return the triple scalar product of \p a, \p b, and \p c
         */
        template<IsArithmetic a_type, DoesNotNarrowlyConvertTo<a_type> b_type, DoesNotNarrowlyConvertTo<b_type> c_type>
        [[nodiscard]] [[maybe_unused]]
        static inline
        value_type triple_scalar_product(const Vector_X<3, a_type>& a, const Vector_X<3, b_type>& b, const Vector_X<3, c_type>& c)
        {
            return a * (b.cross(c));
        }

        /*!
         * Interpolates between \p a and \p b, by the factor \p t. A \p t value of 0.0 will return the vector \p a, and a
         * \p t value of 1.0 will return the vector \p b
         * @param a The first vector to interpolate between
         * @param b The second vector to interpolate between
         * @param t the interpolation value. must be between 0.0 and 1.0
         * @return The interpolated vector
         */
        template<IsArithmetic a_type, DoesNotNarrowlyConvertTo<a_type> b_type, DoesNotNarrowlyConvertTo<a_type> T>
        [[nodiscard]] static
        Vector_X<N, value_type> linear_interpolation(const Vector_X<N, a_type>& a, const Vector_X<N, b_type>& b, T t)
        {
            if(t > 1.0 || t < 0.0)
            {
                throw std::invalid_argument("t must be in the range of [0.0, 1.0]. Given value was: " + std::to_string(t));
            }
            return (a * (1.0 - t)) + (b * t);
        }
    };

    template<size_t N, IsArithmetic value_type, size_t unit_dimension>
    static Vector_X<N, value_type> getUnit_N() {
        Vector_X<N, value_type> result{};
        result[unit_dimension] = static_cast<value_type>(1);
        return result;
    }
}