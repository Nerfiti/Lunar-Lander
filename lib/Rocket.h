#pragma once

#include "Sprite.h"

class Rocket final : public Sprite
{
    public:
        Rocket(const Rect &rect, bool expand = true);
        Rocket(Rect &&rect, bool expand = true);

        enum class EngineMode
        {
            IDLE,
            INCREASE_THRUST,
            DECREASE_THRUST,
            SET_MAX_THRUST,
            SET_NO_THRUST
        };

        enum class RcsEngineMode
        {
            IDLE,
            CW,
            CCW,
            STABILIZE,
        };

        void update(double dt);

        /*
        *   Static preferences of the rocket (getters/setters)
        */
        void set_mass(double mass);
        void set_increasing_thrust_speed(double delta_thrust);
        void set_max_rotation_thrust(double max_rotation_thrust);
        void set_max_thrust(double max_thrust);
        void set_max_fuel(double max_fuel);
        void set_max_hydrazine(double max_fuel);
        void set_fuel_consumption(double fuel_per_thrust);
        void set_hydrazine_consumption(double hydrazine_per_thrust);
        void set_gravity_acceleration(double g_x, double g_y);
        void set_stabilization_treshold(double treshold_speed);

        /*
        *   Dynamic preferences of the rocket (getters/setters)
        */
        void toggle_engine(EngineMode mode);
        void toggle_rcs(RcsEngineMode RCS_mode);

        ///TODO: add other preferences
    private:
        /*
        *   Static preferences of the rocket
        */
        double mass_                         = 1;
        double max_rotation_thrust_          = 1;
        double max_thrust_                   = 100;
        double delta_thrust_                 = 100;
        double max_fuel_                     = 1;
        double fuel_per_thrust_              = 0.00001;
        double max_hydrazine_                = 1;
        double hydrazine_per_thrust_         = 0.1;
        double g_x_                          = 0;
        double g_y_                          = 10;
        double treshold_speed_for_stabilize_ = 0.001;

        /*
        *   Dynamic preferences of the rocket
        */
        double velocity_x_     = 0;
        double velocity_y_     = 0;
        double accel_x_        = 0;
        double accel_y_        = 0;
        double rotation_speed_ = 0;
        double rotation_accel_ = 0;

        double fuel_           = max_fuel_;
        double hydrazine_      = max_hydrazine_;

        double thrust_         = 0;


        EngineMode engine_mode;
        RcsEngineMode rcs_mode_;

        void update_thrust(double dt);
        void update_rotation_accel_();
};