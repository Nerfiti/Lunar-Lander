#include <cmath>

#include "RectTexture.h"

RectTexture::RectTexture(Color color, size_t width, size_t height):
    buffer_(width * height, color),
    width_(width),
    height_(height)
    {}

RectTexture::RectTexture(Color color, Vector2d size):
    buffer_(),
    width_(size.x),
    height_(size.y)
    { buffer_.resize(width_ * height_, color); }

const std::vector<uint32_t> &RectTexture::get_buffer() const
{
    return buffer_;
}

const uint32_t RectTexture::get_pixel_color(size_t id) const
{
    return buffer_[id];
}

const uint32_t RectTexture::get_pixel_color(size_t x, size_t y) const
{
    return buffer_[y * width_ + x];
}

void RectTexture::set_pixel_color(size_t id, uint32_t color)
{
    buffer_[id] = color;
}

void RectTexture::set_pixel_color(size_t x, size_t y, uint32_t color)
{
    buffer_[y * width_ + x] = color;
}

void RectTexture::draw_circle(Color color, Vector2d center, double radius)
{
    double radius_sq = radius * radius;
    for (size_t y = 0; y < height_; ++y)
    {
        int y_rel = static_cast<int>(y) - center.y;
        for (size_t x = 0; x < width_; ++x)
        {
            int x_rel = static_cast<int>(x) - center.x;
            if (x_rel * x_rel + y_rel * y_rel < radius_sq)
                set_pixel_color(x, y, color.blend(get_pixel_color(x, y)));
        }
    }
}

#include <iostream>
void RectTexture::draw_rect(Color color, Vector2d position, Vector2d size, double angle)
{
    double sin = std::sin(angle);
    double cos = std::cos(angle);

    for (size_t y = 0; y < size.y; ++y)
    {
        for (size_t x = 0; x < size.x; ++x)
        {
            size_t x_final = x * cos - y * sin + position.x;
            size_t y_final = x * sin + y * cos + position.y;

            if (x_final >= width_ || y_final >= height_)
                continue;

            set_pixel_color(x_final, y_final, color.blend(get_pixel_color(x_final, y_final)));
            if (x_final + 1 < width_)
                set_pixel_color(x_final + 1, y_final, color.blend(get_pixel_color(x_final, y_final)));

        }
    }
    // size_t y_max = std::min(height_, static_cast<size_t>(size.y));
    // size_t x_max = std::min(width_, static_cast<size_t>(size.x));

    // for (size_t y = position.y; y < y_max; ++y)
    //     for (size_t x = position.x; x < x_max; ++x)
    //         set_pixel_color(x, y, color.blend(get_pixel_color(x, y)));
}

size_t RectTexture::get_width() const
{
    return width_;
}

size_t RectTexture::get_height() const
{
    return height_;
}

Vector2d RectTexture::get_size() const
{
    return Vector2d(width_, height_);
}

