#include <cmath>

#include "Vector.h"

Vector2d::Vector2d():
    Vector2d(0, 0)
    {}

Vector2d::Vector2d(double coords):
    Vector2d(coords, coords)
    {}

Vector2d::Vector2d(double x_coord = 0, double y_coord = 0):
    x(x_coord),
    y(y_coord)
    {}

double Vector2d::dot(const Vector2d &other) const
{
    return x * other.x + y * other.y;
}

double Vector2d::norm_sq() const
{
    return this->dot(*this);
}

double Vector2d::norm() const
{
    return std::sqrt(norm_sq());
}

Vector2d &Vector2d::normalize()
{
    return *this /= norm();
}

Vector2d Vector2d::normal() const
{
    return Vector2d(-y, x);
}

Vector2d &Vector2d::operator+=(const Vector2d &other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vector2d &Vector2d::operator-=(const Vector2d &other)
{
    *this += -other;
    return *this;
}

Vector2d &Vector2d::operator*=(double other)
{
    x *= other;
    y *= other;
    return *this;
}

Vector2d &Vector2d::operator/=(double other)
{
    *this *= (1 / other);
    return *this;
}

Vector2d operator-(const Vector2d &vec)
{
    return Vector2d(-vec.x, -vec.y);
}

Vector2d operator+(const Vector2d &lhs, const Vector2d &rhs)
{
    Vector2d tmp = lhs;
    return tmp += rhs;
}

Vector2d operator-(const Vector2d &lhs, const Vector2d &rhs)
{
    Vector2d tmp = lhs;
    return tmp -= rhs;
}

Vector2d operator*(const Vector2d &vec, double factor)
{
    Vector2d tmp = vec;
    return tmp *= factor;
}

Vector2d operator*(double factor, const Vector2d &vec)
{
    return vec * factor;
}

Vector2d operator/(const Vector2d &vec, double factor)
{
    Vector2d tmp = vec;
    return tmp /= factor;
}
