#include <iostream>
#include <stdlib.h>
#include <memory.h>

#include "Engine.h"
#include "Planet.h"
#include "Rocket.h"

/* 
*  This is an anonymous namespace for game data
*  It probably would be better to use classes structure but
*  since we have four main functions (initialize, act, draw and finalize)
*  it will be just replace these functions by classes methods
*/
namespace
{
    Rocket rocket(RectTexture(Color::White, 100, 100), true);
    Planet planet(SCREEN_WIDTH);
};

void initialize() 
{
    std::srand(time(NULL));
    planet.generate_landscape(100, SCREEN_HEIGHT / 4, 150);

    rocket.set_center(50, 50);
    rocket.move(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void act(float dt)
{
    static bool need_stabilize = false;
    static bool was_return_pressed = false;

    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    if (is_key_pressed(VK_UP))
        rocket.toggle_engine(Rocket::EngineMode::INCREASE_THRUST);
    else if (is_key_pressed(VK_DOWN))
        rocket.toggle_engine(Rocket::EngineMode::DECREASE_THRUST);
    else
        rocket.toggle_engine(Rocket::EngineMode::IDLE);

    if (is_key_pressed(VK_LEFT))
        rocket.toggle_rcs(Rocket::RcsEngineMode::CCW);
    else if (is_key_pressed(VK_RIGHT))
        rocket.toggle_rcs(Rocket::RcsEngineMode::CW);
    else
        rocket.toggle_rcs(need_stabilize ? Rocket::RcsEngineMode::STABILIZE : Rocket::RcsEngineMode::IDLE);

    if (is_key_pressed(VK_RETURN))
    {
        if (!was_return_pressed)
        {
            need_stabilize = !need_stabilize;
            rocket.toggle_rcs(Rocket::RcsEngineMode::STABILIZE);
        }
        was_return_pressed = true;
    }
    else
    {
        was_return_pressed = false;
    }

    rocket.update(dt);
    if (planet.check_collision(rocket.get_collider()))
        std::cerr << "COLLISION\n";

    static const size_t frames_to_show_fps = 100;
    static size_t frame_counter = 0;
    static double delta_time = 0;

    delta_time += dt;
    ++frame_counter;
    if (frame_counter == frames_to_show_fps)
    {
        std::cout << "Fps: " << static_cast<size_t>(frame_counter / delta_time) << '\n';
        delta_time = 0;
        frame_counter = 0;
    }
}

void draw()
{
    auto buffer_as_1D = reinterpret_cast<uint32_t *>(buffer);
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    planet.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
    rocket.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
}

/*
*   Nothing to free because of destructors
*/
void finalize() {}