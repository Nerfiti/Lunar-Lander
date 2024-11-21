#include <cmath>
#include <numbers>

#include "Sprite.h"
#include <iostream>

Sprite::Sprite(const Rect &rect, bool expand):
    rect_(rect),
    center_x_(0),
    center_y_(0),
    pos_x_(0),
    pos_y_(0),
    phi_(0),
    need_sin_cos_update_(false),
    sin_phi_(0),
    cos_phi_(1),
    expand_on_rotate_(expand)
    {}

Sprite::Sprite(Rect &&rect, bool expand):
    rect_(std::move(rect)),
    center_x_(0),
    center_y_(0),
    pos_x_(0),
    pos_y_(0),
    phi_(0),
    need_sin_cos_update_(false),
    sin_phi_(0),
    cos_phi_(1),
    expand_on_rotate_(expand)
    {}

Rect &Sprite::get_rect()
{
    return rect_;
}

const Rect &Sprite::get_rect() const
{
    return rect_;
}

double Sprite::get_sin_phi()
{
    if (!need_sin_cos_update_)
        return sin_phi_;

    sin_phi_ = std::sin(phi_);
    cos_phi_ = std::cos(phi_);
    need_sin_cos_update_ = false;

    return sin_phi_;
}

double Sprite::get_cos_phi()
{
    if (!need_sin_cos_update_)
        return cos_phi_;

    sin_phi_ = std::sin(phi_);
    cos_phi_ = std::cos(phi_);
    need_sin_cos_update_ = false;

    return cos_phi_;
}

void Sprite::set_center(int x, int y)
{
    center_x_ = x;
    center_y_ = y;
}

void Sprite::set_expand(bool expand)
{
    expand_on_rotate_ = expand;
}

void Sprite::move(double x, double y) 
{
    pos_x_ += x;
    pos_y_ += y;
}

void Sprite::rotate(double phi)
{
    phi_ += phi;
    int factor = phi_ / std::numbers::pi;
    phi_ -= factor * 2 * std::numbers::pi;

    need_sin_cos_update_ = true;
}

void Sprite::draw(uint32_t *buffer, size_t width, size_t height)
{
    if (need_sin_cos_update_)
    {
        sin_phi_ = std::sin(phi_);
        cos_phi_ = std::cos(phi_);
    }

    for (int y = 0; y < rect_.get_height(); ++y)
    {
        int y_relative = y - center_y_;
        for (int x = 0; x < rect_.get_width(); ++x)
        {
            int x_relative = x - center_x_;

            double x_real = cos_phi_ * x_relative - sin_phi_ * y_relative + pos_x_;
            double y_real = sin_phi_ * x_relative + cos_phi_ * y_relative + pos_y_;

            Color color = rect_.get_pixel_color(x + y * rect_.get_width());
            if (!expand_on_rotate_)
            {
                if (x_real < 0 || x_real >= width || y_real < 0 || y_real >= height)
                    continue;

                int id_in_screen_buf = x_real + y_real * width;
                buffer[id_in_screen_buf] = color;
            }
            else
            {
                draw_pixel_with_interpolation(buffer, width, height, x_real, y_real, color);
            }
        }
    }
}

void Sprite::draw_pixel_with_interpolation(uint32_t *buffer, size_t width, size_t height, double x, double y, Color color)
{
    if (x < 0 || x >= width || y < 0 || y >= height)
        return;

    size_t x_floor = static_cast<size_t>(x);
    size_t y_floor = static_cast<size_t>(y);
    size_t x_ceil  = x_floor + 1;
    size_t y_ceil  = y_floor + 1;

    double x_frac = x - x_floor;
    double y_frac = y - y_floor;

    int top_horizontal_impact = 2 * 255 * y_frac;
    int right_up_alpha   = std::min(255, static_cast<int>(top_horizontal_impact * x_frac));
    int left_up_alpha    = std::min(255, top_horizontal_impact - right_up_alpha);
    int left_down_alpha  = std::min(255, 255 - right_up_alpha);
    int right_down_alpha = std::min(255, 255 - left_up_alpha);

    buffer[y_floor * width + x_floor] = color.set_alpha(left_down_alpha).blend(buffer[y_floor * width + x_floor]);

    if (x_ceil < width)
        buffer[y_floor * width + x_ceil] = color.set_alpha(right_down_alpha).blend(buffer[y_floor * width + x_ceil]);

    if (y_ceil >= height)
        return;

    buffer[y_ceil * width + x_floor] = color.set_alpha(left_up_alpha).blend(buffer[y_ceil * width + x_floor]);
    buffer[y_ceil * width + x_ceil ] = color.set_alpha(right_up_alpha).blend(buffer[y_ceil * width + x_ceil]);

}