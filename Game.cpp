#include <iostream>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>

#include "Engine.h"
#include "Planet.h"
#include "ProgressBar.h"
#include "Rocket.h"

/* 
*  This is an anonymous namespace for game data
*  It probably would be better to use classes structure but
*  since we have four main functions (initialize, act, draw and finalize)
*  it will be just replace these functions by classes methods
*/
namespace
{
    Rocket rocket(Vector2d(50, 80), true);
    Planet planet(SCREEN_WIDTH, SCREEN_HEIGHT);

    //----------------------------------------------------------------
    // Fuel bars
    //----------------------------------------------------------------
    ///TODO: Draw icons for bars
    constexpr size_t Bars_width  = 100;
    constexpr size_t Bars_height = 20;
    ProgressBar fuel_bar(Sprite(RectTexture(Color::Purple, Bars_height, Bars_height)), 
                                                    Vector2d(SCREEN_WIDTH - Bars_width, 0),
                                                    Vector2d(Bars_width, Bars_height),
                                                    Color::White, Color::Red, 0);

    ProgressBar hydrazine_bar(Sprite(RectTexture(Color::Purple, Bars_height, Bars_height)), 
                                                    Vector2d(SCREEN_WIDTH - Bars_width, 2 * Bars_height),
                                                    Vector2d(Bars_width, Bars_height),
                                                    Color::White, Color::Cyan, 0);

    //----------------------------------------------------------------
    // Game over screens
    //----------------------------------------------------------------
    constexpr size_t game_over_screen_size = 300;
    Sprite win_screen(RectTexture(Color(0, 0, 0, 0), Vector2d(game_over_screen_size)));
    Sprite lose_screen(RectTexture(Color(0, 0, 0, 0), Vector2d(game_over_screen_size)));

    //----------------------------------------------------------------
    // Game over screens showing logic
    //----------------------------------------------------------------
    constexpr double Showing_time = 1.0;
    double cur_showing_time = 0.0;
    bool player_wins = false;
    bool player_lose = false;
};

static void handle_input();
static void key_press_callback(int vk_key_code);
static void key_release_callback(int vk_key_code);
static void handle_collisions(float dt);
static void show_fps(float dt);
static void update_all(float dt);
static void restart();

//----------------------------------------------------------------
// Four main functions to engine call
//----------------------------------------------------------------

void initialize()
{
    std::srand(time(NULL));

    // setup rocket
    //----------------------------------------------------------------
    restart();

    // setup progress bars
    //----------------------------------------------------------------
    fuel_bar.set_max_progress(rocket.get_max_fuel());
    hydrazine_bar.set_max_progress(rocket.get_max_hydrazine());

    // setup win screen
    //----------------------------------------------------------------
    auto &win_screen_texture = win_screen.get_texture();
    win_screen_texture.draw_rect(Color::Green, Vector2d(game_over_screen_size / 2, 0.9 * game_over_screen_size),
                                               Vector2d(game_over_screen_size / 10, game_over_screen_size / 2),
                                               -21 * std::numbers::pi / 16);

    win_screen_texture.draw_rect(Color::Green, Vector2d(game_over_screen_size / 2, 0.9 * game_over_screen_size),
                                               Vector2d(game_over_screen_size / 10, 0.9 * game_over_screen_size),
                                               19 * std::numbers::pi / 16);

    win_screen.set_center(game_over_screen_size / 2, game_over_screen_size / 2);
    win_screen.set_position(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    // setup lose screen
    //----------------------------------------------------------------
    auto &lose_screen_texture = lose_screen.get_texture();
    lose_screen_texture.draw_rect(Color::Red, Vector2d(game_over_screen_size / 8, game_over_screen_size / 6),
                                              Vector2d(game_over_screen_size / 10, game_over_screen_size),
                                              -std::numbers::pi / 4);

    lose_screen_texture.draw_rect(Color::Red, Vector2d(4 * game_over_screen_size / 5, game_over_screen_size / 10),
                                              Vector2d(game_over_screen_size / 10, game_over_screen_size),
                                              std::numbers::pi / 4);

    lose_screen.set_center(game_over_screen_size / 2, game_over_screen_size / 2);
    lose_screen.set_position(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);

    //----------------------------------------------------------------
}

void act(float dt)
{
    handle_input();

    if (player_lose || player_wins)
    {
        cur_showing_time += dt;
        if (cur_showing_time > Showing_time)
        {
            player_lose = player_wins = false;
            cur_showing_time = 0.0f;
            restart();
        }
        return;
    }

    update_all(dt);
    handle_collisions(dt);

    switch (rocket.get_state())
    {
        case Rocket::RocketState::CRASHED:
        {
            player_lose = true;
            break;
        }
        case Rocket::RocketState::LANDED:
        {
            player_wins = true;
            break;
        }
        case Rocket::RocketState::IN_FLIGHT:
        {
            break;
        }
    }

    show_fps(dt);
}

void draw()
{
    auto buffer_as_1D = reinterpret_cast<uint32_t *>(buffer);
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));

    planet.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
    rocket.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);

    fuel_bar.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
    hydrazine_bar.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (player_lose)
        lose_screen.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
    if (player_wins)
        win_screen.draw(buffer_as_1D, SCREEN_WIDTH, SCREEN_HEIGHT);
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

static void handle_collisions(float dt)
{
    static std::vector<CollisionInfo> info;
    static constexpr size_t Probable_max_number_of_mtvs = 8;
    info.reserve(Probable_max_number_of_mtvs);

    const auto &colliders = rocket.get_colliders();
    size_t number_of_colliders = colliders.size();
    for (size_t i = 0; i < number_of_colliders; ++i)
    {
        bool mtv_if_collision = planet.check_collision(colliders[i], info, dt);
        if (mtv_if_collision)
        {
            for (const auto &collision_info : info)
                rocket.apply_collision_response(i, collision_info, dt, info.size());
        }
    }
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

    fuel_bar.set_progress(rocket.get_fuel());
    hydrazine_bar.set_progress(rocket.get_hydrazine());
}

static void restart()
{
    planet.generate_stars();
    planet.generate_landscape(SCREEN_WIDTH >> 5, SCREEN_HEIGHT / 4, 150);

    rocket.set_default_configuration();
    rocket.set_position(SCREEN_WIDTH / 10, SCREEN_HEIGHT / 10);
}