//
// Created by olber on 7/14/2022.
//

#pragma once

#include <array>
#include <vector>
#include "Point_X.h"
#include "Vector_X.h"

namespace linear_algebra_core {

    template<size_t M, size_t N, typename value_type>
    class Matrix_MxN {
    private:
        std::array<Vector_X<N, value_type>, M> m_values{};

        template<size_t Column, size_t... Rows>
        constexpr Vector_X<N, value_type> getColumnHelper(std::index_sequence<Rows...>) const {
            Vector_X<N, value_type> result;
            ((result.template getValue<Rows>() = getValue<Rows, Column>()), ...);
            return result;
        }

    public:
        Matrix_MxN() = default;

        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[maybe_unused]]
        Matrix_MxN(std::initializer_list<std::initializer_list<T>> rows)
        {
            if constexpr (rows.size() != M) {
                throw std::invalid_argument("Matrix_MxN constructor expected a initializer_list of size " + std::to_string(M) + ", but got size " + std::to_string(rows.size()));
            }
            size_t index = 0;
            for(const auto& row : rows) {
                m_values[index++] = Vector_X<N, value_type>(row);
            }
        }

        template<DoesNotNarrowlyConvertTo<value_type> T, size_t... n>
        explicit constexpr Matrix_MxN(const T (&...list)[n])
        {
            size_t index = 0;
            ((m_values[index++] = Vector_X<N, value_type>(list)), ...);
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[maybe_unused]]
        explicit constexpr Matrix_MxN(const std::array<std::array<T, N>, M>& other)
        {
            for(int i = 0; i < M; i++) {
                m_values[i] = Vector_X<N, value_type>{other[i]};
            }
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[maybe_unused]]
        explicit constexpr Matrix_MxN(const std::array<Vector_X<N, T>, M>& other) : m_values{other} { }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[maybe_unused]]
        explicit constexpr Matrix_MxN(const std::array<Vector_X<N, T>, M>&& other) : m_values{other} { }

        ~Matrix_MxN() = default;

        template<DoesNotNarrowlyConvertTo<value_type> T>
        Matrix_MxN(const Matrix_MxN<M, N, T>& other) {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), M, begin());
            }
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        Matrix_MxN(Matrix_MxN<M, N, T>&& other) noexcept : m_values{std::move(other.m_values)} { }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        Matrix_MxN<M, N, T>& operator=(const Matrix_MxN<M, N, T>& other) {
            if(&other != this) [[likely]] {
                std::copy_n(other.cbegin(), M, begin());
            }
            return *this;
        }

        template<DoesNotNarrowlyConvertTo<value_type> T>
        Matrix_MxN<M, N, T>& operator=(Matrix_MxN<M, N, T>&& other) noexcept {
            m_values = std::move(other.m_values);
            return *this;
        }

        auto begin()   { return std::begin(m_values);   }
        auto end()     { return std::end(m_values);     }
        auto rbegin()  { return std::rbegin(m_values);  }
        auto rend()    { return std::rend(m_values);    }
        [[nodiscard]] auto cbegin()  const { return std::cbegin(m_values);  }
        [[nodiscard]] auto cend()    const { return std::cend(m_values);    }
        [[nodiscard]] auto crbegin() const { return std::crbegin(m_values); }
        [[nodiscard]] auto crend()   const { return std::crend(m_values);   }

        /*!
         * @param index index of the desired row
         * @return A copy of the row at the given \p index
         */
        [[nodiscard]] inline Vector_X<N, value_type> operator[](size_t index) const { return m_values[index]; }
        /*!
         * @param index index of the desired row
         * @return A reference to the row at the given \p index
         */
        [[nodiscard]] inline Vector_X<N, value_type>& operator[](size_t index) { return m_values[index]; }

        /*!
         * Exception-throwing version of the index operation
         * @param index index of the desired row
         * @return A copy of the row at the given \p index
         */
        [[nodiscard]] inline Vector_X<N, value_type> at(size_t index) const { return m_values.at(index); }
        /*!
         * Exception-throwing version of the index operation
         * @param index index of the desired row
         * @return A reference to the row at the given \p index
         */
        [[nodiscard]] inline Vector_X<N, value_type>& at(size_t index) { return m_values.at(index); }

        /*!
         * Exception-throwing double indexing operation
         * @param row index of the desired row
         * @param column index of the desired column
         * @return a copy of the value at the given \p row and \p column
         */
        [[nodiscard]] inline value_type at(size_t row, size_t column) const { return m_values.at(row).at(column); }
        /*!
         * Exception-throwing double indexing operation
         * @param row index of the desired row
         * @param column index of the desired column
         * @return a reference to the value at the given \p row and \p column
         */
        [[nodiscard]] inline value_type& at(size_t row, size_t column) { return m_values.at(row).at(column); }

        /*!
         * Compile-time double index operation
         * @tparam Row index of the desired row
         * @tparam Column index of the desired column
         * @return a copy of the value at the given \p row and \p column
         */
        template<size_t Row, size_t Column>
        [[nodiscard]] constexpr inline value_type getValue() const requires ((Row < M) && (Column < N))
        {
            return std::get<Row>(m_values).template getValue<Column>();
        }

        /*!
         * Compile-time double index operation
         * @tparam Row index of the desired row
         * @tparam Column index of the desired column
         * @return A reference to the value at the given \p row and \p column
         */
        template<size_t Row, size_t Column>
        [[nodiscard]] constexpr inline value_type& getValue() requires ((Row < M) && (Column < N))
        {
            return std::get<Row>(m_values).template getValue<Column>();
        }

        /*!
         * Compile-time Row access
         * @tparam Index index of the desired row
         * @return a copy of the row at the given \p Index
         */
        template<size_t Index>
        [[nodiscard]] [[maybe_unused]] constexpr inline
        Vector_X<M, value_type> getRow() const requires (Index < M)
        {
            return std::get<Index>(m_values);
        }

        /*!
         * Compile-time Row access
         * @tparam Index index of the desired row
         * @return a reference to the row at the given \p Index
         */
        template<size_t Index>
        [[nodiscard]] [[maybe_unused]] constexpr inline
        Vector_X<M, value_type>& getRow() requires (Index < M)
        {
            return std::get<Index>(m_values);
        }

        /*!
         * Explicit row access. Mostly just here for code clarity purposes. Throws an exception if \p index is out of bounds
         * @param index index of the desired row
         * @return A copy of the row at the given \p index
         */
        [[nodiscard]] [[maybe_unused]] inline Vector_X<M, value_type> getRow(size_t index) const { return m_values.at(index); }
        /*!
         * Explicit row access. Mostly just here for code clarity purposes. Throws an exception if \p index is out of bounds
         * @param index index of the desired row
         * @return A reference to the row at the given \p index
         */
        [[nodiscard]] [[maybe_unused]] inline Vector_X<M, value_type>& getRow(size_t index) { return m_values.at(index); }

        /*!
         * Compile-time column accessor
         * @tparam Index index of the desired column
         * @return A copy of the column at the given \p Index
         */
        template<size_t Index>
        [[nodiscard]] constexpr Vector_X<N, value_type> getColumn() const requires (Index < N)
        {
            return getColumnHelper<Index>(std::make_index_sequence<N>{});
        }

        /*!
         * Column accessor. Will throw an exception of \p index is out of bounds
         * @param index index of the desired column
         * @return A copy of the column at the given \p index
         */
        [[nodiscard]] Vector_X<N, value_type> getColumn(size_t index) const
        {
            if(index >= N) {
                throw std::out_of_range("given index (" + std::to_string(index) + ") was out of range.");
            }
            Vector_X<N, value_type> result;
            for(int i = 0; i < M; i++) {
                result[i] = m_values[i][index];
            }
            return result;
        }

        /*!
         * @param rhs Matrix to multiply by
         * @return The result of the multiplication
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Matrix_MxN<M, M, value_type> operator*(const Matrix_MxN<N, M, other_type>& rhs) const
        {
            Matrix_MxN<M, M, value_type> result;
            for(int i = 0; i < M; i++) {
                for(int j = 0; j < N; j++) {
                    result[i][j] = m_values[i] * rhs.getColumn(j);
                }
            }
            return result;
        }

        /*!
         * @param rhs Matrix to multiply this matrix by
         * @return a reference to this matrix
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        inline Matrix_MxN<M, N, value_type>& operator*=(const Matrix_MxN<N, M, other_type>& rhs) requires (M == N)
        {
            (*this) = (*this) * rhs;
            return (*this);
        }

        /*!
         * Row-vector multiplication
         * @param lhs Vector to be multiplied
         * @param rhs Matrix to multiply the vector by
         * @return The resulting vector
         */
        template<IsArithmetic vector_type, DoesNotNarrowlyConvertTo<vector_type> matrix_type>
        [[nodiscard]] friend
        Vector_X<N, vector_type> operator*(const Vector_X<M, vector_type>& lhs, const Matrix_MxN<M, N, matrix_type>& rhs)
        {
            Vector_X<N, vector_type> result;
            for(int i = 0; i < N; i++) {
                result[i] = lhs * rhs.getColumn(i);
            }
            return result;
        }

        /*!
         * Column-Vector multiplication
         * @param rhs The vector to multiply by
         * @return The result of the multiplication
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Vector_X<M, value_type> operator*(const Vector_X<N, other_type>& rhs) const
        {
            Vector_X<M, value_type> result;
            for(int i = 0; i < M; i++) {
                result[i] = m_values[i] * rhs;
            }
            return result;
        }

        /*!
         * @tparam T Type of \p scalar. Must be an arithmetic type
         * @param scalar value to scale the matrix by
         * @return The scaled matrix
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Matrix_MxN<M, N, value_type> operator*(const T& scalar) const
        {
            Matrix_MxN<M, N, value_type> result;
            for(int i = 0; i < M; i++) {
                result[i] = m_values[i] * scalar;
            }
            return result;
        }

        /*!
         * @tparam T Type of \p scalar. Must be an arithmetic type.
         * @param scalar Value to scale the matrix by.
         * @return A reference to this matrix
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        [[nodiscard]] Matrix_MxN<M, N, value_type>& operator*=(const T& scalar)
        {
            for(auto& row : m_values) {
                row *= scalar;
            }
            return *this;
        }

        /*!
         * @param rhs Matrix to add to this
         * @return The result of the addition
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Matrix_MxN<M, N, value_type> operator+(const Matrix_MxN<M, N, other_type>& rhs) const
        {
            Matrix_MxN<M, N, value_type> result;
            for(int i = 0; i < M; i++) {
                result[i] = m_values[i] + rhs[i];
            }
            return result;
        }

        /*!
         * @param rhs matrix to add to this one
         * @return a reference to this matrix
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Matrix_MxN<M, N, value_type>& operator+=(const Matrix_MxN<M, N, other_type>& rhs)
        {
            for(int i = 0; i < M; i++) {
                m_values[i] += rhs[i];
            }
            return (*this);
        }

        /*!
         * @param rhs matrix to subtract
         * @return the result of the subtraction
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        [[nodiscard]] Matrix_MxN<M, N, value_type> operator-(const Matrix_MxN<M, N, other_type>& rhs) const
        {
            Matrix_MxN<M, N, value_type> result;
            for(int i = 0; i < M; i++) {
                result[i] = m_values[i] - rhs[i];
            }
            return result;
        }

        /*!
         * @param rhs Matrix to subtract this one by
         * @return a reference to this matrix
         */
        template<DoesNotNarrowlyConvertTo<value_type> other_type>
        Matrix_MxN<M, N, value_type>& operator-=(const Matrix_MxN<M, N, other_type>& rhs)
        {
            for(int i = 0; i < M; i++) {
                m_values[i] -= rhs[i];
            }
            return (*this);
        }

        /*!
         * @return the string representation of this matrix
         */
        [[nodiscard]] std::string to_string() const
        {
            std::string result = "{ \n";
            for(int i = 0; i < (M - 1); i++) {
                result += "\t" + m_values[i] + ",\n";
            }
            if constexpr (M > 0) {
                result + "\t" + m_values[M - 1] + "\n";
            }
            result + "}";
            return result;
        }

        /*!
         * ostream operation for matrices
         * @param out ostream to place the matrix into
         * @param rhs matrix to be serialized
         * @return a reference to \p out
         */
        friend std::ostream& operator<<(std::ostream& out, const Matrix_MxN<M, N, value_type>& rhs)
        {
            out << rhs.to_string();
            return out;
        }

        /*!
         * @return A transposed version of this matrix
         */
        [[maybe_unused]]
        Matrix_MxN<N, M, value_type> getTransposed() const
        {
            Matrix_MxN<N, M, value_type> result;
            for(int i = 0; i < N; i++) {
                result[i] = getColumn(i);
            }
            return result;
        }

        /*!
         * In-place matrix transposition. Matrix must be square.
         * @return a reference to this matrix
         */
        [[maybe_unused]]
        Matrix_MxN<M, N, value_type>& transpose() requires (M == N)
        {
            for(int i = 1; i < M; i++) {
                for(int j = 0; j < i; j++) {
                    std::swap(m_values[i][j], m_values[j][i]);
                }
            }
            return (*this);
        }
    };
}