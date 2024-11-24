#include <numbers>

#include "Rocket.h"

Rocket::Rocket(Vector2d size, bool expand):
    sprites_(),
    colliders_(),
    transform_(size)
{
    set_default_configuration(true);

    double one_by_sqrt2 = 1.0 / std::sqrt(2);

    // Fire of engines (no colliders)
    auto rocket_fire = draw_rocket_fire();
    fire_sprite_id_ = setup_part(rocket_fire, Vector2d(rocket_fire.get_width() / 2, 0), Vector2d(), 0, false).first;

    // Rocket roof (square rotated on 45 degree)
    auto rocket_roof = RectTexture(Color::Red, size.x * one_by_sqrt2, size.x * one_by_sqrt2);
    setup_part(rocket_roof, rocket_roof.get_size() / 2, Vector2d(0, -size.y / 2 + size.x / 4), -std::numbers::pi / 4);

    // Rocket body with area for roof. (size.x / 4) - diagonal of roof square
    auto rocket_body = draw_rocket_body();
    setup_part(rocket_body, size / 2, Vector2d(0, size.x / 4), 0);

    // Rocket landing legs
    auto rocket_leg = RectTexture(0xff424242, size.x / 8, size.y / 4);

    left_leg_collider_id_  = setup_part(rocket_leg, Vector2d(rocket_leg.get_width() / 2, 0),
                                                    Vector2d(-size.x / 2, size.y / 2),
                                                    std::numbers::pi / 8).second;

    right_leg_collider_id_ = setup_part(rocket_leg, Vector2d(rocket_leg.get_width() / 2, 0),
                                                    Vector2d( size.x / 2, size.y / 2),
                                                    -std::numbers::pi / 8).second;
}

void Rocket::set_default_configuration(bool first_time)
{
    if (!first_time)
        sprites_relative_positions_[fire_sprite_id_] += Vector2d(0, -thrust_ / max_thrust_ * transform_.get_size().y / 2);

    mass_                         = 1;
    max_rotation_thrust_          = 1;
    max_thrust_                   = 100;
    delta_thrust_                 = 100;
    max_fuel_                     = 1;
    fuel_per_thrust_              = 0.001;
    max_hydrazine_                = 1;
    hydrazine_per_thrust_         = 0.1;
    treshold_speed_for_stabilize_ = 0.001;
    free_fall_accel_              = Vector2d(0, 20);

    set_angle(0);
    set_position(Vector2d());

    velocity_       = Vector2d(0, 0);
    acceleration_   = Vector2d(0, 0);
    rotation_speed_ = 0;
    rotation_accel_ = 0;
    fuel_           = max_fuel_;
    hydrazine_      = max_hydrazine_;
    thrust_         = 0;

    engine_mode_ = EngineMode::IDLE;
    rcs_mode_ = RcsEngineMode::IDLE;
    prev_passive_mode_ = RcsEngineMode::IDLE;
    state_ = RocketState::IN_FLIGHT;

    left_leg_landed  = false;
    right_leg_landed = false;
}

void Rocket::update(double dt)
{
    double old_thrust = thrust_;
    update_rotation_accel_();
    update_thrust(dt);

    sprites_relative_positions_[fire_sprite_id_] += Vector2d(0, (thrust_ - old_thrust) / max_thrust_ * transform_.get_size().y / 2);
    Vector2d direction(transform_.get_sin(), -transform_.get_cos());
    acceleration_ = free_fall_accel_ + thrust_ * direction / (mass_ + fuel_);

    fuel_ -= thrust_ * dt * fuel_per_thrust_;
    hydrazine_ -= std::abs(rotation_accel_) * (mass_ + fuel_) * dt * hydrazine_per_thrust_;

    move(velocity_ * dt);
    velocity_ += acceleration_ * dt;

    rotate(rotation_speed_ * dt);
    rotation_speed_ += rotation_accel_ * dt;

    if (left_leg_landed && right_leg_landed)
        state_ = RocketState::LANDED;
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
void Rocket::set_gravity_acceleration   (double g_x, double g_y)      { free_fall_accel_      = Vector2d(g_x, g_y);     }

void Rocket::set_stabilization_treshold(double treshold_speed) { treshold_speed_for_stabilize_ = treshold_speed; }

double Rocket::get_mass         () const { return mass_;          }

double Rocket::get_max_fuel     () const { return max_fuel_;      }

double Rocket::get_max_hydrazine() const { return max_hydrazine_; }

/*
*   Dynamic preferences of the rocket (getters/setters)
*/
Rocket::RocketState Rocket::get_state() const { return state_;     }

double Rocket::get_fuel              () const { return fuel_;      }

double Rocket::get_hydrazine         () const { return hydrazine_; }

/*
*   Methods for rocket control
*/
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
    if (velocity_.norm_sq() > Min_speed_norm_sq_to_destroy)
    {
        state_ = RocketState::CRASHED;
        return;
    }
    static constexpr double Min_mtv_norm_sq_to_react = 1e-15;

    Vector2d mtv = info.mtv / number_of_collisions;
    const Vector2d &normal = info.normal;

    if (std::abs(normal.x) > Cos_max_inclination_angle_to_landing)
    {
        state_ = RocketState::CRASHED;
        return;
    }

    if (collider_id == left_leg_collider_id_)
        left_leg_landed = true;
    if (collider_id == right_leg_collider_id_)
        right_leg_landed = true;

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

    velocity_ += delta_v;
    Vector2d horizontal_velocity = velocity_.dot(normal.normal()) * normal.normal();

    static constexpr double friction_factor = 0.5;
    velocity_ -= horizontal_velocity * friction_factor;

    acceleration_ -= acceleration_.dot(mtv) * mtv / mtv.norm_sq();

    // Some physics
    static constexpr int Inertia_factor = 3;
    Vector2d rel_pos = most_remote_point - transform_.get_position();
    double delta_rot_speed = -Inertia_factor * rel_pos.dot((delta_v - horizontal_velocity * friction_factor).normal());
    delta_rot_speed /= rect_size.norm_sq() + Inertia_factor * (collider_position.norm_sq() + rel_pos.norm_sq());
    delta_rot_speed /= number_of_collisions;

    rotation_speed_ += delta_rot_speed;
}

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

void Rocket::set_position(Vector2d position)
{
    move(position - transform_.get_position());
}

void Rocket::set_position(double x, double y)
{
    set_position(Vector2d(x, y));
}

void Rocket::set_angle(double angle)
{
    rotate(angle - transform_.get_angle());
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

std::pair<size_t, size_t> Rocket::setup_part(RectTexture texture, Vector2d center, 
                                             Vector2d relative_position, double angle, bool need_collider)
{
    Vector2d size = Vector2d(texture.get_width(), texture.get_height());
    Sprite sprite(texture);
    sprite.set_center(center.x, center.y);
    sprite.rotate(angle);
    sprites_.push_back(sprite);
    sprites_relative_positions_.push_back(relative_position);

    if (need_collider)
    {
        RectCollider collider(size);
        collider.set_pivot(center);
        collider.rotate(angle);
        colliders_.push_back(collider);
        colliders_relative_positions_.push_back(relative_position);
    }

    return {sprites_relative_positions_.size() - 1, colliders_relative_positions_.size() - 1};
}

RectTexture Rocket::draw_rocket_body()
{
    Vector2d size = transform_.get_size();

    // Area for roof with diagonal (size.x / 4)
    RectTexture body(Color::White, size.x, size.y - size.x / 4);
    size = body.get_size();

    double external_window_radius = size.x / 4;
    double internal_window_radius = size.x / 5;
    Vector2d window_center = size / 2;

    body.draw_circle(Color::Black, window_center, external_window_radius);
    body.draw_circle(Color::Cyan, window_center, internal_window_radius);

    return body;
}

RectTexture Rocket::draw_rocket_fire()
{
    Vector2d fire_size = Vector2d(transform_.get_size().x, transform_.get_size().y / 2);
    RectTexture fire(Color(0, 0, 0, 0), fire_size);

    for (size_t y = 0; y < fire_size.y; ++y)
    {
        size_t row_len     = 0.7 * (fire_size.y - y) * fire_size.x / fire_size.y;
        size_t left_bound  = (fire_size.x - row_len) / 2;
        size_t right_bound = fire_size.x - (fire_size.x - row_len) / 2;

        for (size_t x = left_bound + 1; x < right_bound; ++x)
        {
            Color color = Color::Red;
            double intensity = std::numbers::e - std::exp(y / fire_size.y);
            color.set_red  (std::min(255, (int)(255 * intensity) + rand() % 50));
            color.set_green(std::min(255, (int)(150 * intensity) + rand() % 20));
            color.set_blue (std::min(255, (int)(50  * intensity) + rand() % 10));

            fire.set_pixel_color(x, y, color);

        }
    }

    return fire;
}