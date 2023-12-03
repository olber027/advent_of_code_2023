#pragma once
#include "LinearAlgebraTypeTraits.h"
#include "Vector_X.h"
#include "Point_X.h"

namespace linear_algebra_core
{
    template<size_t N, IsArithmetic value_type>
    class Ray
    {
    private:
        Point_X<N, value_type>  m_origin;
        Vector_X<N, value_type> m_direction;
        Vector_X<N, value_type> m_inverse_direction;

    public:
        Ray() = default;
        explicit Ray(const Point_X<N, value_type>& start, const Vector_X<N, value_type>& dir) :
            m_origin{start},
            m_direction{dir.getUnitVector()} ,
            m_inverse_direction{m_direction.getInverse()}
            { }
        ~Ray() = default;
        Ray(const Ray& other) = default;
        Ray(Ray&& other) noexcept = default;
        Ray& operator=(const Ray& other) = default;
        Ray& operator=(Ray&& other) noexcept = default;

        /*!
         * Calculate the point at getOrigin() + (\p scalar * getDirection())
         * @tparam T Type of \p scalar. Must be an arithmetic type.
         * @param scalar value to scale the direction by.
         * @return The point returned by the above calculation
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Point_X<N, value_type> operator*(T scalar) const {
            return m_origin + (m_direction * scalar);
        }

        /*!
         * Calculate the point at getOrigin() + (\p scalar * getDirection())
         * @tparam T Type of \p scalar. Must be an arithmetic type.
         * @param scalar value to scale the direction by.
         * @return The point returned by the above calculation
         */
        template<DoesNotNarrowlyConvertTo<value_type> T>
        Point_X<N, value_type> at(T scalar) const {
            return (*this) * scalar;
        }

        /*!
         * @return the origin of the ray
         */
        [[maybe_unused]] const Point_X<N, value_type>& getOrigin() { return m_origin; }
        /*!
         * @return the origin of the ray
         */
        [[nodiscard]] Point_X<N, value_type> getOrigin() const { return m_origin; }
        /*!
         * Sets the origin to \p new_origin
         * @param new_origin the new origin for the ray
         */
        [[maybe_unused]] void setOrigin(const Point_X<N, value_type>& new_origin) { m_origin = new_origin; }

        /*!
         * @return the direction of the ray
         */
        const Vector_X<N, value_type>& getDirection() { return m_direction; }
        /*!
         * @return the direction of the ray
         */
        [[nodiscard]] Vector_X<N, value_type> getDirection() const { return m_direction; }
        /*!
         * sets the direction of the ray to the \p new_direction
         * @param new_direction the new direction for the ray
         */
        [[maybe_unused]] void setDirection(const Vector_X<N, value_type>& new_direction) {
            m_direction = new_direction.getUnitVector();
            m_inverse_direction = 1.0 / m_direction;
        }

        /*!
         * @return the inverse of the direction
         */
        const Vector_X<N, value_type>& getInverse() { return m_inverse_direction; }

        /*!
         * @return the inverse of the direction
         */
        [[nodiscard]] Vector_X<N, value_type> getInverse() const { return m_inverse_direction; }
    };
}