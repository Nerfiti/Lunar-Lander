#include <algorithm>
#include <numeric>

#include "Collider.h"

Segment::Segment(double first_x, double first_y, double second_x, double second_y):
    a_x(first_x),
    a_y(first_y),
    b_x(second_x),
    b_y(second_y)
    {}

Segment::Segment(Vector2d first, Vector2d second):
    Segment(first.x, first.y, second.x, second.y)
    {}

Vector2d Segment::get_normal() const
{
    return Vector2d(-(a_y - b_y), a_x - b_x);
}

AABB::AABB(double left_bound, double right_bound, double top_bound, double bottom_bound):
    left(left_bound),
    right(right_bound),
    top(top_bound),
    bottom(bottom_bound)
    {}

AABB::AABB(const Segment &segment): AABB()
{
    int left   = std::min(segment.a_x, segment.b_x);
    int right  = std::max(segment.a_x, segment.b_x);
    int top    = std::max(segment.a_y, segment.b_y);
    int bottom = std::min(segment.a_y, segment.b_y);
}

AABB::AABB(const Vector2d &first_point, const Vector2d &second_point): 
    AABB(Segment(first_point, second_point))
    {}

bool AABB::is_intersect(const AABB &segment) const
{
    if (left > segment.right  || right < segment.left)
        return false;
    if (bottom  > segment.top || top < segment.bottom)
        return false;

    return true;
}

const AABB &Collider::get_AABB() const { return box_; }

bool Collider::check_AABB_AABB_collision(const AABB &segment) const
{
    return box_.is_intersect(segment);
}

bool Collider::check_AABB_segment_collision(const Segment &segment) const
{
    if (!box_.is_intersect(AABB(segment)))
        return false;

    static constexpr size_t Number_of_vertices = 4;
    std::array<Vector2d, Number_of_vertices> vertices;

    vertices[0] = Vector2d(box_.left , box_.top   );
    vertices[1] = Vector2d(box_.left , box_.bottom);
    vertices[2] = Vector2d(box_.right, box_.top   );
    vertices[3] = Vector2d(box_.right, box_.bottom);

    Vector2d target_axis = segment.get_normal();
    double min_coord = target_axis.dot(vertices[0]);
    double max_coord = target_axis.dot(vertices[0]);
    for (size_t i = 1; i < Number_of_vertices; ++i)
    {
        double coord = target_axis.dot(vertices[i]);
        min_coord = std::min(min_coord, coord);
        max_coord = std::max(max_coord, coord);
    }

    if (max_coord < 0 || min_coord > 0)
        return false;

    return true;
}
