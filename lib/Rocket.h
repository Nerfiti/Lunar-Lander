#pragma once

#include "RectCollider.h"
#include "Sprite.h"

class Rocket final
{
    public:
        Rocket(Vector2d size, bool expand = true);

    public:
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
            CW,
            CCW,
            IDLE,
            STABILIZE,
            PREV_PASSIVE_MODE
        };

    public:
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
        bool is_alive() const;
        void toggle_engine(EngineMode mode);
        void toggle_rcs(RcsEngineMode RCS_mode);
        void switch_rcs_stabilization_mode();

        /*
        *   Some methods to impact the rocket transformation without
        *   using engine (for collisions etc.)
        */
        void apply_collision_response(size_t collider_id, const CollisionInfo &info, float dt, size_t number_of_collisions);

        void move(double x, double y);
        void move(Vector2d offset);
        void rotate(double angle);

        const std::vector<RectCollider> &get_colliders() const;

        /*
        *   Other
        */
        void draw(uint32_t *buffer, size_t width, size_t height);
        void set_center(Vector2d center);
        void set_center(double x, double y);

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
        double treshold_speed_for_stabilize_ = 0.001;
        Vector2d g_                          = Vector2d(0, 20);

        RectTransform transform_;
        /*
        *   Dynamic preferences of the rocket
        */
        Vector2d velocity_      = Vector2d(0, 0);
        Vector2d acceleration_  = Vector2d(0, 0);
        double rotation_speed_ = 0;
        double rotation_accel_ = 0;

        double fuel_           = max_fuel_;
        double hydrazine_      = max_hydrazine_;

        double thrust_         = 0;

        EngineMode engine_mode_ = EngineMode::IDLE;
        RcsEngineMode rcs_mode_ = RcsEngineMode::IDLE;
        RcsEngineMode prev_passive_mode_ = RcsEngineMode::IDLE;

        void update_thrust(double dt);
        void update_rotation_accel_();

        std::vector<Sprite> sprites_;
        std::vector<Vector2d> sprites_relative_positions_;
        void setup_sprites(bool expand);

        std::vector<RectCollider> colliders_;
        std::vector<Vector2d> colliders_relative_positions_;
        void setup_colliders();

        bool is_alive_ = true;

        const double Min_speed_norm_sq_to_destroy = 1000;
};