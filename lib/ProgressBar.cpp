#include "ProgressBar.h"

ProgressBar::ProgressBar(Sprite icon, Vector2d position, Vector2d size, Color background, Color progress_color, double max_progress):
    background_    (background),
    progress_color_(progress_color),
    position_      (position),
    size_          (size),
    progress_      (0.0),
    max_progress_  (max_progress),
    icon_          (icon)
{
    Vector2d icon_size = icon_.get_transform().get_size();
    icon_.set_center(icon_size.x / 2, 0);
    icon_.set_position(position_ - Vector2d(icon_size.x, 0));
}

void ProgressBar::set_progress(double progress)
{
    progress_ = progress;
}

void ProgressBar::set_max_progress(double max_progress)
{
    max_progress_ = max_progress;
}

void ProgressBar::draw(uint32_t *buffer, size_t width, size_t height)
{
    icon_.draw(buffer, width, height);

    size_t y_min = std::max(static_cast<size_t>(position_.y), 0UL);
    size_t y_max = std::min(static_cast<size_t>(position_.y + size_.y), height);

    size_t x_min = std::max(static_cast<size_t>(position_.x), 0UL);
    size_t x_max = std::min(static_cast<size_t>(position_.x + size_.x), width);
    size_t x_divider = x_min;
    if (progress_ > 0 && max_progress_ > 0)
        x_divider = (progress_ / max_progress_) * (x_max - x_min) + x_min;

    for (size_t y = y_min; y < y_max; ++y)
    {
        for (size_t x = x_min; x < x_divider; ++x)
            buffer[y * width + x] = progress_color_;

        for (size_t x = x_divider; x < x_max; ++x)
            buffer[y * width + x] = background_;
    }
}