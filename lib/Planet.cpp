#include <random>

#include "Planet.h"

Planet::Planet(size_t width, size_t height, Color color):
    ground_(),
    width_(width),
    height_(height),
    color_(color)
    {}

///TODO: Generate flat areas to landing
void Planet::generate_landscape(size_t pixels_per_line, uint32_t height_mean, uint32_t height_std)
{
    for (size_t x = 0; x < width_; x += pixels_per_line)
    {
        uint32_t y = height_ - ((std::rand() % (2 * height_std)) - height_std + height_mean);
        ground_.add_point(x, y);
    }
}

void Planet::draw(uint32_t *buffer, size_t width, size_t height)
{
    for (size_t x = 0; x < width; ++x)
    {
        size_t cur_height = std::min(static_cast<uint32_t>(height), ground_.get_height(x));
        for (size_t y = height - 1; y > cur_height; --y)
        {
            buffer[y * width + x] = color_;
        }
    }
}

bool Planet::check_collision(const RectCollider &collider, std::vector<CollisionInfo> &info, float dt) const
{
    return ground_.check_collision(collider, info);
}