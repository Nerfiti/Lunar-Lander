#include "Rocket.h"

Rocket::Rocket(const Rect &rect, bool expand):
    Sprite(rect, expand)
    {}

Rocket::Rocket(Rect &&rect, bool expand):
    Sprite(std::move(rect), expand)
    {}

#include <iostream>
void Rocket::update(double dt)
{
    update_rotation_accel_();
    update_thrust(dt);
    accel_x_ = g_x_ + thrust_ * get_sin_phi() / (mass_ + fuel_);
    accel_y_ = g_y_ - thrust_ * get_cos_phi() / (mass_ + fuel_);

    fuel_ -= thrust_ * dt * fuel_per_thrust_;
    hydrazine_ -= rotation_accel_ * (mass_ + fuel_) * dt * hydrazine_per_thrust_;

    move(velocity_x_ * dt, velocity_y_ * dt);
    velocity_x_ += accel_x_ * dt;
    velocity_y_ += accel_y_ * dt;

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
    g_x_ = g_x; 
    g_y_ = g_y;
}

void Rocket::set_stabilization_treshold(double treshold_speed) { treshold_speed_for_stabilize_ = treshold_speed; }

/*
*   Dynamic preferences of the rocket (getters/setters)
*/
void Rocket::toggle_engine(   EngineMode mode) { engine_mode = mode; }
void Rocket::toggle_rcs   (RcsEngineMode mode) {   rcs_mode_ = mode; }


void Rocket::update_thrust(double dt)
{
    if (fuel_ <= 0)
    {
        thrust_ = 0;
        return;
    }

    switch (engine_mode)
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