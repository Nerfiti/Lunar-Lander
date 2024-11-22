#include <iostream>

#include "RectTexture.h"

RectTexture::RectTexture(Color color, size_t width, size_t height):
    buffer_(width * height, color),
    width_(width),
    height_(height)
    {}

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

size_t RectTexture::get_width() const
{
    return width_;
}

size_t RectTexture::get_height() const
{
    return height_;
}
