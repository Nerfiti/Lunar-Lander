#pragma once

#include "Vector.h"

class RectTransform
{
    public:
        RectTransform(const Vector2d &size, const Vector2d &pos = Vector2d(), const Vector2d &pivot = Vector2d(), double angle = 0.0);

        void move(double x, double y);
        void move(Vector2d offset);

        void rotate(double angle);

        void set_position(Vector2d position);
        void set_angle(double angle);
        void set_pivot(Vector2d pivot);

        Vector2d get_position() const;
        Vector2d get_size() const;
        double get_angle() const;
        Vector2d get_pivot() const;

        double get_sin() const;
        double get_cos() const;

        Vector2d transform_point(const Vector2d &point) const;

    private:
        Vector2d pos_;
        Vector2d size_;
        Vector2d pivot_;
        double angle_;

        mutable bool need_sin_update_;
        mutable bool need_cos_update_;

        mutable double sin_angle_;
        mutable double cos_angle_;
};