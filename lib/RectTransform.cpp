#include <cmath>
#include <numbers>

#include "RectTransform.h"

RectTransform::RectTransform(const Vector2d &size, const Vector2d &pos, const Vector2d &pivot, double angle):
    pos_(pos),
    size_(size),
    pivot_(pivot),
    angle_(angle),
    need_cos_update_(true),
    need_sin_update_(true),
    sin_angle_(0.0),
    cos_angle_(0.0)
    {}

void RectTransform::move(double x, double y)
{
    move(Vector2d(x, y)); 
}

void RectTransform::move(Vector2d offset)
{
    pos_ += offset; 
}

void RectTransform::rotate(double angle)
{
    angle_ += angle;
    int sign = angle_ >= 0 ? 1 : -1;
    int factor = sign * angle_ / std::numbers::pi;
    angle_ -= sign * (factor + 1) / 2 * 2 * std::numbers::pi;

    need_sin_update_ = true;
    need_cos_update_ = true;
}

void RectTransform::set_position(Vector2d position) 
{
    pos_ = position; 
}

void RectTransform::set_angle(double angle)
{ 
    angle = angle_; 
}

void RectTransform::set_pivot(Vector2d pivot)
{
    pivot_ = pivot; 
}

Vector2d RectTransform::get_position() const
{
    return pos_; 
}

Vector2d RectTransform::get_size() const
{
    return size_;
}

double RectTransform::get_angle() const
{ 
    return angle_;
}

Vector2d RectTransform::get_pivot() const
{
    return pivot_;
}

double RectTransform::get_sin() const
{
    if (!need_sin_update_)
        return sin_angle_;

    sin_angle_ = std::sin(angle_);
    need_sin_update_ = false;

    return sin_angle_;
}

double RectTransform::get_cos() const
{
    if (!need_cos_update_)
        return cos_angle_;

    cos_angle_ = std::cos(angle_);
    need_cos_update_ = false;

    return cos_angle_;
}

Vector2d RectTransform::transform_point(const Vector2d &point) const
{
    double relative_x = point.x - pivot_.x;
    double relative_y = point.y - pivot_.y;

    return Vector2d(
        get_cos() * relative_x - get_sin() * relative_y + pos_.x,
        get_sin() * relative_x + get_cos() * relative_y + pos_.y
    );
}
