#include <cstdlib>
#include <numeric>

#include "RectCollider.h"

RectCollider::RectCollider(Vector2d size, Vector2d position, Vector2d pivot, double angle):
    Collider(),
    transform_(size, position, pivot, angle),
    need_update_vertices(true)
    {}

bool RectCollider::check_collision(const Segment& other) const
{
    if (!box_.is_intersect(AABB(other)))
        return false;

    static constexpr size_t Vertices_num_in_segment = 2;
    std::array<Vector2d, Vertices_num_in_segment> segment_vertices;
    segment_vertices[0] = Vector2d(other.a_x, other.a_y);
    segment_vertices[1] = Vector2d(other.b_x, other.b_y);

    return !SAT_check<Number_of_vertices, Vertices_num_in_segment>(get_vertices(), segment_vertices);
}

bool RectCollider::check_collision(const RectCollider& other) const
{
    if (!box_.is_intersect(other.get_AABB()))
        return false;

    return !SAT_check<Number_of_vertices, Number_of_vertices>(get_vertices(), other.get_vertices());
}

void RectCollider::move(double x, double y)
{
    transform_.move(x, y);
    box_.left   += x;
    box_.right  += x;
    box_.top    += y;
    box_.bottom += y;

    for (auto &vertex : transformed_vertices_)
        vertex += Vector2d(x, y);
}

void RectCollider::move(Vector2d offset)
{
    move(offset.x, offset.y);
}


///TODO: small angles rotate optimization (don't need to update vertices after each rotation)
void RectCollider::rotate(double angle)
{
    transform_.rotate(angle);
    update_AABB();
    need_update_vertices = true;
}


void RectCollider::set_position(Vector2d position)
{
    move(position - transform_.get_position());
}

void RectCollider::set_angle(double angle)
{
    transform_.set_angle(angle);
    update_AABB();
    need_update_vertices = true;
}

void RectCollider::set_pivot(Vector2d pivot)
{
    transform_.set_pivot(pivot);
    need_update_vertices = true;
}

const RectTransform &RectCollider::get_transform() const
{
    return transform_;
}

const RectCollider::polygon_t &RectCollider::get_vertices() const
{
    update_vertices();
    return transformed_vertices_;
}

void RectCollider::update_AABB()
{
    const polygon_t &vertices = get_vertices();

    double left   = vertices[0].x;
    double right  = vertices[0].x;
    double top    = vertices[0].y;
    double bottom = vertices[0].y;
    for (const Vector2d &vertex : vertices)
    {
        left   = std::min(left  , vertex.x);
        right  = std::max(right , vertex.x);
        top    = std::max(top   , vertex.y);
        bottom = std::min(bottom, vertex.y);
    }

    box_ = AABB(left, right, top, bottom);
}

template<size_t first_vertices, size_t second_vertices>
bool RectCollider::SAT_check(const std::array<Vector2d, first_vertices> &lhs,
                             const std::array<Vector2d, second_vertices> &rhs) const
{
    if (SAT_check_only_first_edges<first_vertices, second_vertices>(lhs, rhs))
        return true;
    if (SAT_check_only_first_edges<second_vertices, first_vertices>(rhs, lhs))
        return true;

    return false;
}

template<size_t first_vertices, size_t second_vertices>
bool RectCollider::SAT_check_only_first_edges(const std::array<Vector2d, first_vertices> &lhs, 
                                              const std::array<Vector2d, second_vertices> &rhs) const
{
    if (first_vertices == 0)
        return true;

    for (size_t edge_id = 0; edge_id < first_vertices - 1; ++edge_id)
    {
        Vector2d target_axis = (lhs[edge_id + 1] - lhs[edge_id]).normal();
        if (SAT_check_one_axis(lhs, rhs, target_axis))
            return true;
    }

    Vector2d target_axis = lhs[0] - lhs[first_vertices - 1];
    return SAT_check_one_axis(lhs, rhs, target_axis);
}

template<size_t first_vertices, size_t second_vertices>
bool RectCollider::SAT_check_one_axis(const std::array<Vector2d,  first_vertices> &lhs,
                                     const std::array<Vector2d, second_vertices> &rhs,
                                     Vector2d target_axis) const
{
    Vector2d first  = get_min_max_projection_coord(lhs, target_axis);
    Vector2d second = get_min_max_projection_coord(rhs, target_axis);

    if (first.x > second.y || first.y < second.x)
        return true;

    return false;
}

template<size_t number_of_vertices>
Vector2d RectCollider::get_min_max_projection_coord(const std::array<Vector2d, number_of_vertices> &array, Vector2d target_axis) const
{
    double min = target_axis.dot(array[0]);
    double max = target_axis.dot(array[0]);

    for (size_t i = 1; i < number_of_vertices; ++i)
    {
        double coord = target_axis.dot(array[i]);
        min = std::min(min, coord);
        min = std::max(max, coord);
    }

    return Vector2d(min, max);
}


void RectCollider::update_vertices() const
{
    if (!need_update_vertices)
        return;

    Vector2d size = transform_.get_size();
    transformed_vertices_[0] = transform_.transform_point(Vector2d(  0   ,   0   ));
    transformed_vertices_[1] = transform_.transform_point(Vector2d(  0   , size.y));
    transformed_vertices_[2] = transform_.transform_point(Vector2d(size.x,   0   ));
    transformed_vertices_[3] = transform_.transform_point(Vector2d(size.x, size.y));

    need_update_vertices = false;
}