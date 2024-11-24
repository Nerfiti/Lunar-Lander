#pragma once

#include "Vector.h"

struct Segment final
{
    Segment(double first_x, double first_y, double second_x, double second_y);
    Segment(Vector2d first, Vector2d second);

    double a_x;
    double a_y;

    double b_x;
    double b_y;

    Vector2d get_normal() const;
};

struct AABB final
{
    AABB(double left_bound = 0, double right_bound = 0, double top_bound = 0, double bottom_bound = 0);
    AABB(const Segment &segment);
    AABB(const Vector2d &first_point, const Vector2d &second_point);

    double left   = 0;
    double right  = 0;
    double top    = 0;
    double bottom = 0;

    bool is_intersect(const AABB &other) const;
};

class RectCollider;

class Collider
{
    public:
        virtual ~Collider() = default;

        const AABB &get_AABB() const;

        bool check_AABB_AABB_collision(const AABB &other) const;
        bool check_AABB_segment_collision(const Segment &other) const;

        virtual std::pair<bool, Vector2d> check_collision(const Segment& other) const = 0;
        virtual std::pair<bool, Vector2d> check_collision(const RectCollider& other) const = 0;

        friend class RectCollider;
    private:
        AABB box_;
};

struct CollisionInfo
{
    Vector2d mtv;
    Vector2d normal;
};