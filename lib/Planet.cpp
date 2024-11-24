#include <random>

#include "Planet.h"

Planet::Planet(size_t width, size_t height, Color color):
    ground_(),
    width_(width),
    height_(height),
    color_(color)
    {}

void Planet::generate_landscape(size_t pixels_per_line, uint32_t height_mean, uint32_t height_std)
{
    ground_.clear();
    static constexpr size_t Areas_count = 3;
    static constexpr size_t Min_area_size = 80;
    static constexpr size_t Max_area_size = 100;

    size_t min_height = height_mean >= height_std ? height_mean - height_std : 0;
    size_t max_height = std::min(height_mean + height_std, static_cast<uint32_t>(height_));

    std::array<uint32_t, Areas_count> area_positions;
    std::array<uint32_t, Areas_count> area_sizes;
    for (size_t i = 0; i < Areas_count; ++i)
    {
        size_t left_border  = i * width_ / Areas_count;
        size_t right_border = left_border + width_ / Areas_count - 1;
        area_positions[i] = generate_rand_from_to(left_border, right_border - Min_area_size);
        area_sizes[i] = generate_rand_from_to(Min_area_size, std::min(right_border - area_positions[i], Max_area_size));

        uint32_t area_height = height_ - generate_rand_from_to(min_height, max_height);
        ground_.add_point(area_positions[i], area_height);
        ground_.add_point(area_positions[i] + area_sizes[i] - 1, area_height);
    }

    size_t cur_area = 0;
    for (size_t x = 0; x <= width_; x += pixels_per_line)
    {
        double y_tmp = height_ - height_mean + height_std * std::sin(x / static_cast<double>(width_) * 2 * M_PI) * std::cos(x);
        size_t y = y_tmp + generate_rand_from_to(-static_cast<int32_t>(height_std) / 10, height_std / 10);

        if (cur_area < Areas_count && x > area_positions[cur_area] + area_sizes[cur_area])
            ++cur_area;

        if (cur_area >= Areas_count || x < area_positions[cur_area])
        {
            ground_.add_point(x, y);
        }
    }
}

void Planet::generate_stars()
{
    size_t x_min = 1;
    size_t x_max = width_ - 2;
    size_t y_min = 1;
    size_t y_max = height_ - 2;
    for (size_t i = 0; i < Stars_count; ++i)
    {
        stars_[i] = Vector2d(generate_rand_from_to(x_min, x_max), generate_rand_from_to(y_min, y_max));
    }
}

void Planet::draw(uint32_t *buffer, size_t width, size_t height)
{
    for (Vector2d star_pos : stars_)
    {
        for (int y_rel = -1; y_rel <= 1; ++y_rel)
        {
            for (int x_rel = -1; x_rel <= 1; ++x_rel)
            {
                if (x_rel * y_rel != 0)
                    continue;

                size_t x = star_pos.x + x_rel;
                size_t y = star_pos.y + y_rel;
                if (x < width_ && y < height_)
                    buffer[y * width + x] = Color::White;
            }
        }
    }

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

int32_t Planet::generate_rand_from_to(int32_t from, int32_t to) const
{
    ++to;
    if (to <= from)
        return from;

    return rand() % (to - from) + from;
}