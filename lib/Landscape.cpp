#include "Landscape.h"

Landscape::Landscape():
    ground_points_(),
    prev_point_(ground_points_.end())
    {}

void Landscape::add_point(uint32_t x, uint32_t y)
{
    ground_points_[x] = y;
    if (ground_points_.size() == 1)
        prev_point_ = ground_points_.begin();
}

uint32_t Landscape::get_height(uint32_t x)
{
    if (ground_points_.empty())
        return 0;

    int64_t cache = try_in_cache(x);
    if (cache >= 0)
        return cache;

    auto right = ground_points_.lower_bound(x);
    auto left  = right;
    if (right == ground_points_.begin())
        left = ground_points_.end();
    else
        --left;

    prev_point_ = left;
    if (right == ground_points_.end())
    {
        prev_point_ = left;
        return (*left).second;
    }
    if (left == ground_points_.end())
    {
        prev_point_ = ground_points_.begin();
        return (*right).second;
    }

    const auto &[x_left, y_left] = *left;
    const auto &[x_right, y_right] = *right;
    return interpolate(x, x_left, x_right, y_left, y_right);
}

uint32_t Landscape::get_height_naive(uint32_t x) const
{
    if (ground_points_.empty())
        return 0;

    int64_t cache = try_in_cache(x);
    if (cache >= 0)
        return cache;

    auto right = ground_points_.lower_bound(x);
    auto left  = right;
    if (right == ground_points_.begin())
        left = ground_points_.end();
    else
        --left;

    if (right == ground_points_.end())
        return (*left).second;
    if (left == ground_points_.end())
        return (*right).second;

    const auto &[x_left, y_left] = *left;
    const auto &[x_right, y_right] = *right;
    return interpolate(x, x_left, x_right, y_left, y_right);
}

bool Landscape::check_collision(const RectCollider &collider, std::vector<CollisionInfo> &info) const
{
    info.clear();

    int x_min = collider.get_AABB().left;
    int x_max = collider.get_AABB().right;

    if (x_max < 0)
        return false;

    auto end_indicator = ground_points_.upper_bound(x_max);
    if (end_indicator == ground_points_.begin())
        return false;
    if (end_indicator != ground_points_.end())
        ++end_indicator;

    auto it = ground_points_.begin();
    auto prev_it = it++;

    if (x_min >= 0)
    {
        auto it = ground_points_.lower_bound(x_min);
        auto prev_it = it;
        if (it != ground_points_.begin())
            --prev_it;
        else
            ++it;
    }

    bool collision = false;

    while (it != end_indicator)
    {
        const auto &[x_1, y_1] = *prev_it;
        Vector2d first_point(x_1, y_1);
        const auto &[x_2, y_2] = *it;
        Vector2d second_point(x_2, y_2);

        Segment segment(first_point, second_point);

        if (collider.check_AABB_segment_collision(segment))
        {
            std::pair<bool, Vector2d> mtv_if_collision = collider.check_collision(segment);
            if (mtv_if_collision.first)
            {
                collision = true;
                Vector2d mtv = mtv_if_collision.second.y > 0 ? -mtv_if_collision.second : mtv_if_collision.second;
                Vector2d normal = segment.get_normal().normalize();
                normal = normal.y > 0 ? -normal : normal;
                info.emplace_back(mtv, normal);
            }
        }
        ++it;
        ++prev_it;
    }

    return collision;
}

void Landscape::clear()
{
    ground_points_.clear();
}

int64_t Landscape::try_in_cache(uint32_t x) const
{
    const auto &[x_left, y_left] = *prev_point_;
    if (x_left > x)
        return -1;

    auto next_point = prev_point_;
    ++next_point;
    const auto &[x_right, y_right] = *next_point;
    if (x_right < x)
        return -1;

    return interpolate(x, x_left, x_right, y_left, y_right);
}

uint32_t Landscape::interpolate(uint32_t x, uint32_t left, uint32_t right, uint32_t left_height, uint32_t right_height) const
{
    double t = static_cast<double>(right - x) / (right - left);
    // t = -2 * t * t * t + 3 * t * t;
    return left_height * t + right_height * (1 - t);
}