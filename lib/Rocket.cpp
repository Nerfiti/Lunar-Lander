#include <cmath>

#include "Rocket.h"

Rocket::Rocket(Vector2d size, bool expand):
    sprites_(),
    colliders_(),
    transform_(size)
{
    setup_sprites(expand);
    setup_colliders();
}

#include <iostream>
void Rocket::update(double dt)
{
    double old_thrust = thrust_;
    update_rotation_accel_();
    update_thrust(dt);

    sprites_relative_positions_[0] += Vector2d(0, (thrust_ - old_thrust) / max_thrust_ * transform_.get_size().y / 2);
    Vector2d direction(transform_.get_sin(), -transform_.get_cos());
    acceleration_ = g_ + thrust_ * direction / (mass_ + fuel_);

    fuel_ -= thrust_ * dt * fuel_per_thrust_;
    hydrazine_ -= rotation_accel_ * (mass_ + fuel_) * dt * hydrazine_per_thrust_;

    move(velocity_ * dt);
    velocity_ += acceleration_ * dt;

    rotate(rotation_speed_ * dt);
    rotation_speed_ += rotation_accel_ * dt;
}

/*
*   Static preferences of the rocket (getters/setters)
*/
void Rocket::set_mass                   (double mass)                 { mass_                 = mass;                   }
void Rocket::set_increasing_thrust_speed(double delta_thrust)         { delta_thrust_         = delta_thrust;           }
void Rocket::set_max_rotation_thrust    (double max_rotation_thrust)  { max_rotation_thrust_  = max_rotation_thrust;    }
void Rocket::set_max_thrust             (double max_thrust)           { max_thrust_           = max_thrust;             }
void Rocket::set_max_fuel               (double max_fuel)             { max_fuel_             = max_fuel;               }
void Rocket::set_max_hydrazine          (double max_fuel)             { max_hydrazine_        = max_fuel;               }
void Rocket::set_fuel_consumption       (double fuel_per_thrust)      { fuel_per_thrust_      = fuel_per_thrust;        }
void Rocket::set_hydrazine_consumption  (double hydrazine_per_thrust) { hydrazine_per_thrust_ = hydrazine_per_thrust_;  }

void Rocket::set_gravity_acceleration(double g_x, double g_y)
{
    g_ = Vector2d(g_x, g_y);
}

void Rocket::set_stabilization_treshold(double treshold_speed) { treshold_speed_for_stabilize_ = treshold_speed; }

/*
*   Dynamic preferences of the rocket (getters/setters)
*/
bool Rocket::is_alive() const
{
    return is_alive_;
}

void Rocket::toggle_engine(EngineMode mode) { engine_mode_ = mode; }

void Rocket::toggle_rcs(RcsEngineMode mode) 
{
    switch (mode)
    {
        case RcsEngineMode::PREV_PASSIVE_MODE:
        {
            rcs_mode_ = prev_passive_mode_;
            return;
        }
        case RcsEngineMode::IDLE:
        case RcsEngineMode::STABILIZE:
        {
            prev_passive_mode_ = mode;
            //fallthrough
        }
        case RcsEngineMode::CCW:
        case RcsEngineMode::CW:
        {
            rcs_mode_ = mode;
            return;
        }
    }
}

void Rocket::switch_rcs_stabilization_mode()
{
    if (prev_passive_mode_ == RcsEngineMode::IDLE)
    {
        rcs_mode_ = prev_passive_mode_ = RcsEngineMode::STABILIZE;
    }
    else
    {
        rcs_mode_ = prev_passive_mode_ = RcsEngineMode::IDLE;
    }
}

#include <iostream>
void Rocket::apply_collision_response(size_t collider_id, const CollisionInfo &info, float dt, size_t number_of_collisions)
{
    // if (velocity_.norm_sq() > Min_speed_norm_sq_to_destroy)
        // is_alive_ = false;

    static constexpr double Min_mtv_norm_sq_to_react = 1e-15;

    Vector2d mtv = info.mtv / number_of_collisions;
    const Vector2d &normal = info.normal;

    if (mtv.norm_sq() < Min_mtv_norm_sq_to_react)
        return;

    move(mtv / number_of_collisions);

    const RectCollider &collider = colliders_[collider_id];
    Vector2d collider_position = colliders_relative_positions_[collider_id];

    const auto &vertices = collider.get_vertices();
    Vector2d most_remote_point = vertices[0];
    double dot_in_cache = most_remote_point.dot(normal);

    for (size_t i = 1; i < RectCollider::Number_of_vertices; ++i)
    {
        double cur_dot = vertices[i].dot(normal);
        if (cur_dot < dot_in_cache)
        {
            most_remote_point = vertices[i];
            dot_in_cache = cur_dot;
        }
    }

    Vector2d dir_to_center = transform_.get_position() - most_remote_point;
    double distance_to_center = dir_to_center.norm();
    dir_to_center /= distance_to_center;

    Vector2d delta_v = -velocity_.dot(mtv) * mtv / mtv.norm_sq();

    if (delta_v.dot(mtv) < 0)
        return;


    const auto &rect_size = collider.get_transform().get_size();

    // Some physics
    double delta_rot_speed = -3 * (most_remote_point - transform_.get_position()).dot(delta_v.normal());
    delta_rot_speed /= rect_size.norm_sq() + 3 * collider_position.norm_sq();
    delta_rot_speed /= number_of_collisions;

    velocity_ += delta_v;
    acceleration_ -= acceleration_.dot(mtv) * mtv / mtv.norm_sq();

    rotation_speed_ += delta_rot_speed;

    rotation_speed_ += delta_v.dot(normal) * velocity_.dot(normal.normal()) / 10000;

}

#include <iostream>
void Rocket::move(double x, double y)
{
    size_t sprites_num = std::min(sprites_.size(), sprites_relative_positions_.size());
    for (size_t i = 0; i < sprites_num; ++i)
        sprites_[i].set_position(transform_.transform_point(sprites_relative_positions_[i]));

    size_t colliders_num = std::min(colliders_.size(), colliders_relative_positions_.size());
    for (size_t i = 0; i < colliders_num; ++i)
        colliders_[i].set_position(transform_.transform_point(colliders_relative_positions_[i]));

    transform_.move(x, y);
}

void Rocket::move(Vector2d offset)
{
    move(offset.x, offset.y);
}

void Rocket::rotate(double angle)
{
    size_t sprites_num = std::min(sprites_.size(), sprites_relative_positions_.size());
    for (size_t i = 0; i < sprites_num; ++i)
    {
        sprites_[i].rotate(angle);
        sprites_[i].set_position(transform_.transform_point(sprites_relative_positions_[i]));
    }

    size_t colliders_num = std::min(colliders_.size(), colliders_relative_positions_.size());
    for (size_t i = 0; i < colliders_num; ++i)
    {
        colliders_[i].rotate(angle);
        colliders_[i].set_position(transform_.transform_point(colliders_relative_positions_[i]));
    }

    transform_.rotate(angle);
}

const std::vector<RectCollider> &Rocket::get_colliders() const
{
    return colliders_;
}

void Rocket::draw(uint32_t *buffer, size_t width, size_t height)
{
    for (const auto &sprite : sprites_)
        sprite.draw(buffer, width, height);
}

void Rocket::set_center(Vector2d center)
{
    set_center(center.x, center.y);
}

void Rocket::set_center(double x, double y)
{
    transform_.set_pivot(Vector2d(x, y));
}

void Rocket::update_thrust(double dt)
{
    if (fuel_ <= 0)
    {
        thrust_ = 0;
        return;
    }

    switch (engine_mode_)
    {
        case EngineMode::SET_MAX_THRUST:
        {
            thrust_ = max_thrust_;
            break;
        }
        case EngineMode::SET_NO_THRUST:
        {
            thrust_ = 0;
            break;
        }
        case EngineMode::INCREASE_THRUST:
        {
            thrust_ = std::min(thrust_ + delta_thrust_ * dt, max_thrust_);
            break;
        }
        case EngineMode::DECREASE_THRUST:
        {
            thrust_ = std::max(thrust_ - delta_thrust_ * dt, 0.0);
            break;
        }
        case EngineMode::IDLE:
        {
            break;
        }
    }
}

void Rocket::update_rotation_accel_()
{
    if (hydrazine_ <= 0)
    {
        rotation_accel_ = 0;
        return;
    }

    switch (rcs_mode_)
    {
        case RcsEngineMode::CCW:
        {
            rotation_accel_ = -max_rotation_thrust_ / (mass_ + fuel_);
            break;
        }
        case RcsEngineMode::CW:
        {
            rotation_accel_ = max_rotation_thrust_ / (mass_ + fuel_);
            break;
        }
        case RcsEngineMode::IDLE:
        case RcsEngineMode::PREV_PASSIVE_MODE:
        {
            rotation_accel_ = 0;
            break;
        }
        case RcsEngineMode::STABILIZE:
        {
            if (rotation_speed_ > treshold_speed_for_stabilize_)
                rotation_accel_ = -max_rotation_thrust_ / (mass_ + fuel_);
            else if (rotation_speed_ < -treshold_speed_for_stabilize_)
                rotation_accel_ = max_rotation_thrust_ / (mass_ + fuel_);
            else
                rotation_accel_ = 0;
            break;
        }
    }
}

///TODO: REMOVE COPYPASTE
///TODO: Draw some rocket
void Rocket::setup_sprites(bool expand)
{
    ///TODO: understand magic numbers and remove it
    Vector2d size = transform_.get_size();
    double one_by_sqrt2 = 1.0 / std::sqrt(2);

    Sprite fire(RectTexture(Color::Yellow, size.x / 2, size.y / 2));
    fire.set_center(size.x / 4, 0);
    sprites_.push_back(fire);
    sprites_relative_positions_.push_back(Vector2d(0, 0));

    Sprite main_roof(RectTexture(Color::Red, one_by_sqrt2 * size.x, one_by_sqrt2 * size.x));
    main_roof.set_center(one_by_sqrt2 * size.x / 2, one_by_sqrt2 * size.x / 2);
    main_roof.rotate(std::numbers::pi / 4);
    sprites_.push_back(main_roof);
    sprites_relative_positions_.push_back(Vector2d(0, -(size.y - size.x / 2) / 2));

    Sprite main(RectTexture(Color::White, size.x, size.y - size.x / 4));
    main.set_center(size.x / 2, size.y / 2);
    sprites_.push_back(main);
    sprites_relative_positions_.push_back(Vector2d(0, size.x / 4));

    Sprite left_foot(RectTexture(0xff424242, size.x / 5, size.y / 3));
    left_foot.set_center(size.x / 10, 0);
    left_foot.rotate(std::numbers::pi / 8);
    sprites_.push_back(left_foot);
    sprites_relative_positions_.push_back(Vector2d(-size.x / 2, size.y / 2));

    Sprite right_foot(RectTexture(0xff424242, size.x / 5, size.y / 3));
    right_foot.set_center(size.x / 10, 0);
    right_foot.rotate(-std::numbers::pi / 8);
    sprites_.push_back(right_foot);
    sprites_relative_positions_.push_back(Vector2d(size.x / 2, size.y / 2));
}

void Rocket::setup_colliders()
{
    Vector2d size = transform_.get_size();
    double one_by_sqrt2 = 1.0 / std::sqrt(2);

    RectCollider main(Vector2d(size.x, size.y - size.x / 4));
    main.set_pivot(Vector2d(size.x / 2, size.y / 2));
    colliders_.push_back(main);
    colliders_relative_positions_.push_back(Vector2d(0, size.x / 4));

    RectCollider main_roof(Vector2d(one_by_sqrt2 * size.x, one_by_sqrt2 * size.x));
    main_roof.set_pivot(Vector2d(one_by_sqrt2 * size.x / 2, one_by_sqrt2 * size.x / 2));
    main_roof.rotate(std::numbers::pi / 4);
    colliders_.push_back(main_roof);
    colliders_relative_positions_.push_back(Vector2d(0, -(size.y - size.x / 2) / 2));

    RectCollider left_foot(Vector2d(size.x / 5, size.y / 3));
    left_foot.set_pivot(Vector2d(size.x / 10, 0));
    left_foot.rotate(std::numbers::pi / 4);
    colliders_.push_back(left_foot);
    colliders_relative_positions_.push_back(Vector2d(-size.x / 2, size.y / 2));

    RectCollider right_foot(Vector2d(size.x / 5, size.y / 3));
    right_foot.set_pivot(Vector2d(size.x / 10, 0));
    right_foot.rotate(-std::numbers::pi / 4);
    colliders_.push_back(right_foot);
    colliders_relative_positions_.push_back(Vector2d(size.x / 2, size.y / 2));
}