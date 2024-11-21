#include <iostream>

#include "Rect.h"

Rect::Rect(Color color, size_t width, size_t height):
    buffer_(width * height, color),
    width_(width),
    height_(height)
    {}

// Rect::Rect(Rect &&other):
//     buffer_(std::move(other.buffer_)),
//     width_ (other.width_),
//     height_(other.height_)
//     {}

// Rect &Rect::operator=(Rect &&other)
// {
//     buffer_ = std::move(other.buffer_);
//     width_  = std::move(other.width_);
//     height_ = std::move(other.height_);

//     return *this;
// }

const std::vector<uint32_t> &Rect::get_buffer() const
{
    return buffer_;
}

const uint32_t Rect::get_pixel_color(size_t id) const
{
    return buffer_[id];
}

const uint32_t Rect::get_pixel_color(size_t x, size_t y) const
{
    return buffer_[y * width_ + x];
}

void Rect::set_pixel_color(size_t id, uint32_t color)
{
    buffer_[id] = color;
}

void Rect::set_pixel_color(size_t x, size_t y, uint32_t color)
{
    buffer_[y * width_ + x] = color;
}

size_t Rect::get_width() const
{
    return width_;
}

size_t Rect::get_height() const
{
    return height_;
}
