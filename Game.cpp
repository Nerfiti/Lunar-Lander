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
    Rocket rocket(RectTexture(Color::White, 50, 80), true);
    Planet planet(SCREEN_WIDTH, SCREEN_HEIGHT);
};

static void handle_input();
static void key_press_callback(int vk_key_code);
static void key_release_callback(int vk_key_code);
static void handle_collisions();
static void show_fps(float dt);
static void update_all(float dt);

//----------------------------------------------------------------
// Four main functions to engine call
//----------------------------------------------------------------

void initialize() 
{
    std::srand(time(NULL));
    planet.generate_landscape(50, SCREEN_HEIGHT / 4, 150);

    rocket.set_center(25, 40);
    rocket.move(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
}

void act(float dt)
{
    handle_input();

    update_all(dt);
    handle_collisions();

    show_fps(dt);
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

//-----------------------------------------------------------------
//  There are some stuff functions for game
//-----------------------------------------------------------------

#define check_key(VK_KEY)                                           \
    {                                                               \
        static bool was_##VK_KEY##_pressed = false;                 \
        bool is_##VK_KEY##_pressed_now = is_key_pressed(VK_KEY);    \
        if (is_##VK_KEY##_pressed_now && !was_##VK_KEY##_pressed)   \
        {                                                           \
            key_press_callback(VK_KEY);                             \
            was_##VK_KEY##_pressed = true;                          \
        }                                                           \
        if (!is_##VK_KEY##_pressed_now && was_##VK_KEY##_pressed)   \
        {                                                           \
            key_release_callback(VK_KEY);                           \
            was_##VK_KEY##_pressed = false;                         \
        }                                                           \
    }

static void handle_input()
{
    check_key(VK_DOWN  );
    check_key(VK_UP    );
    check_key(VK_LEFT  );
    check_key(VK_RIGHT );
    check_key(VK_RETURN);
    check_key(VK_SPACE );
    check_key(VK_ESCAPE);
}

#undef check_key

static void key_press_callback(int vk_key_code)
{
    switch (vk_key_code)
    {
        case VK_ESCAPE:
        {
            schedule_quit_game();
            break;
        }
        case VK_LEFT:
        {
            rocket.toggle_rcs(Rocket::RcsEngineMode::CCW);
            break;
        }
        case VK_RIGHT:
        {
            rocket.toggle_rcs(Rocket::RcsEngineMode::CW);
            break;
        }
        case VK_UP:
        {
            rocket.toggle_engine(Rocket::EngineMode::INCREASE_THRUST);
            break;
        }
        case VK_DOWN:
        {
            rocket.toggle_engine(Rocket::EngineMode::DECREASE_THRUST);
            break;
        }
        case VK_RETURN:
        {
            rocket.switch_rcs_stabilization_mode();
            rocket.toggle_rcs(Rocket::RcsEngineMode::PREV_PASSIVE_MODE);
        }
        default:
        {
            break;
        }
    }
}

static void key_release_callback(int vk_key_code)
{
    switch (vk_key_code)
    {
        case VK_LEFT:
        case VK_RIGHT:
        {
            rocket.toggle_rcs(Rocket::RcsEngineMode::PREV_PASSIVE_MODE);
            break;
        }
        case VK_UP:
        case VK_DOWN:
        {
            rocket.toggle_engine(Rocket::EngineMode::IDLE);
            break;
        }
        default:
        {
            break;
        }
    }
}

static void handle_collisions()
{
    if (planet.check_collision(rocket.get_collider()))
        std::cerr << "COLLISION\n";
}

static void show_fps(float dt)
{
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

static void update_all(float dt)
{
    rocket.update(dt);
}