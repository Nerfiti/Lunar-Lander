#pragma once

#include <vector>

#include "Color.h"

class Rect final
{
    public:
        Rect(Color color, size_t width, size_t height);

        const std::vector<uint32_t> &get_buffer() const;
        const uint32_t get_pixel_color(size_t id) const;
        const uint32_t get_pixel_color(size_t x, size_t y) const;

        void set_pixel_color(size_t id, uint32_t color);
        void set_pixel_color(size_t x, size_t y, uint32_t color);

        size_t get_width () const;
        size_t get_height() const;

    private:
        std::vector<uint32_t> buffer_;
        size_t width_;
        size_t height_;
};