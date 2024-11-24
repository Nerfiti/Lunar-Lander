#pragma once

#include "Sprite.h"

class ProgressBar final
{
    public:
        ProgressBar(Sprite icon, Vector2d position, Vector2d size, Color background, Color progress, double max_progress);

        void set_progress(double progress);
        void set_max_progress(double max_progress);

        void draw(uint32_t *buffer, size_t width, size_t height);

    private:
        Color background_;
        Color progress_color_;
        Vector2d position_;
        Vector2d size_;

        double progress_;
        double max_progress_;

        Sprite icon_;
};