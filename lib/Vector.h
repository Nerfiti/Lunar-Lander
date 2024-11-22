#pragma once

struct Vector2d final
{
    explicit Vector2d(double x_coord = 0, double y_coord = 0);

    double x;
    double y;

    double dot(const Vector2d &other) const;
    double norm_sq() const;
    double norm() const;

    Vector2d normal() const;

    Vector2d &operator+=(const Vector2d &other);
    Vector2d &operator-=(const Vector2d &other);

    Vector2d &operator*=(double other);
    Vector2d &operator/=(double other);
};

Vector2d operator-(const Vector2d &vec);

Vector2d operator+(const Vector2d &lhs, const Vector2d &rhs);
Vector2d operator-(const Vector2d &lhs, const Vector2d &rhs);

Vector2d operator*(const Vector2d &vec, double factor);
Vector2d operator*(double factor, const Vector2d &vec);

Vector2d operator/(const Vector2d &vec, double factor);