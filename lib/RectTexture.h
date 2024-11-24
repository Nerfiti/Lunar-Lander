#pragma once

#include <vector>

#include "Color.h"
#include "Vector.h"

class RectTexture final
{
    public:
        RectTexture(Color color, size_t width, size_t height);
        RectTexture(Color color, Vector2d size);

        const std::vector<uint32_t> &get_buffer() const;
        const uint32_t get_pixel_color(size_t id) const;
        const uint32_t get_pixel_color(size_t x, size_t y) const;

        void set_pixel_color(size_t id, uint32_t color);
        void set_pixel_color(size_t x, size_t y, uint32_t color);

        void draw_circle(Color color, Vector2d center, double radius);
        void draw_rect(Color color, Vector2d position, Vector2d size, double angle);


        size_t get_width () const;
        size_t get_height() const;
        Vector2d get_size() const;

    private:
        std::vector<uint32_t> buffer_;
        size_t width_;
        size_t height_;
};