#include <cmath>
#include <numbers>

#include "Sprite.h"

Sprite::Sprite(const RectTexture &rect, bool expand):
    transform_(Vector2d(rect.get_width(), rect.get_height())),
    rect_(rect),
    expand_on_rotate_(expand)
    {}

Sprite::Sprite(RectTexture &&rect, bool expand):
    transform_(Vector2d(rect.get_width(), rect.get_height())),
    rect_(std::move(rect)),
    expand_on_rotate_(expand)
    {}

RectTexture &Sprite::get_rect()
{
    return rect_;
}

const RectTexture &Sprite::get_rect() const
{
    return rect_;
}

double Sprite::get_sin_phi()
{
    return transform_.get_sin();
}

double Sprite::get_cos_phi()
{
    return transform_.get_cos();
}

void Sprite::set_center(int x, int y)
{
    transform_.set_pivot(Vector2d(x, y));
}

void Sprite::set_expand(bool expand)
{
    expand_on_rotate_ = expand;
}

void Sprite::move(double x, double y)
{
    transform_.move(x, y);
}

void Sprite::rotate(double phi)
{
    transform_.rotate(phi);
}

void Sprite::draw(uint32_t *buffer, size_t width, size_t height)
{
    for (int y = 0; y < rect_.get_height(); ++y)
    {
        for (int x = 0; x < rect_.get_width(); ++x)
        {
            Color color = rect_.get_pixel_color(x + y * rect_.get_width());

            Vector2d real_position = transform_.transform_point(Vector2d(x, y));
            double x_real = real_position.x;
            double y_real = real_position.y;

            if (!expand_on_rotate_)
            {
                if (x_real < 0 || x_real >= width || y_real < 0 || y_real >= height)
                    continue;

                uint32_t id_in_screen_buf = static_cast<uint32_t>(x_real) + static_cast<uint32_t>(y_real) * width;
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

    // int top_horizontal_impact = 2 * 255 * y_frac;
    // int right_up_alpha   = std::min(255, static_cast<int>(top_horizontal_impact * x_frac));
    // int left_up_alpha    = std::min(255, top_horizontal_impact - right_up_alpha);
    // int left_down_alpha  = std::min(255, 255 - right_up_alpha);
    // int right_down_alpha = std::min(255, 255 - left_up_alpha);

    buffer[y_floor * width + x_floor] = color;//.set_alpha(left_down_alpha);

    if (x_frac > 0.5 && x_ceil < width)
        buffer[y_floor * width + x_ceil] = color;//.set_alpha(right_down_alpha);

    // if (y_ceil >= height)
    //     return;

    // buffer[y_ceil * width + x_floor] = color;//.set_alpha(left_up_alpha);
    // buffer[y_ceil * width + x_ceil ] = color;//.set_alpha(right_up_alpha);

}