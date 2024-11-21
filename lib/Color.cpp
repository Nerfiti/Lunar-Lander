#include "Color.h"

Color::Color(uint32_t color):
    color_(color)
    {}

Color::Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
    color_(0)
    { set_color(r, g, b, a); }

Color &Color::set_red(uint8_t red)
{
    color_ &= ~Red_mask_;
    color_ |= static_cast<uint32_t>(red) << Red_offset_;
    return *this;
}

Color &Color::set_green(uint8_t green)
{
    color_ &= ~Green_mask_;
    color_ |= static_cast<uint32_t>(green) << Green_offset_;
    return *this;
}

Color &Color::set_blue(uint8_t blue)
{
    color_ &= ~Blue_mask_;
    color_ |= static_cast<uint32_t>(blue) << Blue_offset_;
    return *this;
}

Color &Color::set_alpha(uint8_t alpha)
{
    color_ &= ~Alpha_mask_;
    color_ |= static_cast<uint32_t>(alpha) << Alpha_offset_;
    return *this;
}

Color &Color::set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    set_red  (r);
    set_green(g);
    set_blue (b);
    set_alpha(a);
    return *this;
}

uint8_t Color::get_red() const
{
    return (color_ & Red_mask_) >> Red_offset_;
}

uint8_t Color::get_green() const
{
    return (color_ & Green_mask_) >> Green_offset_;
}

uint8_t Color::get_blue() const
{
    return (color_ & Blue_mask_) >> Blue_offset_;
}

uint8_t Color::get_alpha() const
{
    return (color_ & Alpha_mask_) >> Alpha_offset_;
}

uint32_t Color::get_color() const
{
    return color_;
}

Color::operator uint32_t() const
{
    return get_color();
}

// Assumes that background is no transparent
Color Color::blend(Color background) const
{
    auto alpha_1 = get_alpha();
    Color result = Color(
        (get_red  () * alpha_1 + background.get_red  () * (255 - alpha_1)) >> 8,
        (get_green() * alpha_1 + background.get_green() * (255 - alpha_1)) >> 8,
        (get_blue () * alpha_1 + background.get_blue () * (255 - alpha_1)) >> 8
    );

    return result;
}
