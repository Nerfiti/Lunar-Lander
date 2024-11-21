#pragma once

#include "Rect.h"

class Sprite
{
    public:
        Sprite(const Rect &rect, bool expand = true);
        Sprite(Rect &&rect, bool expand = true);
        virtual ~Sprite() = default;

        Rect &get_rect();
        const Rect &get_rect() const;

        double get_sin_phi();
        double get_cos_phi();

        void set_center(int x, int y);
        void set_expand(bool expand);

        void move(double x, double y);
        void rotate(double phi);
        void draw(uint32_t *buffer, size_t width, size_t height);

    protected:
        double pos_x_;
        double pos_y_;
        double phi_;

    private:

        Rect rect_;
        int center_y_;
        int center_x_;
        bool need_sin_cos_update_;
        double sin_phi_;
        double cos_phi_;

        bool expand_on_rotate_;

        void draw_pixel_with_interpolation(uint32_t *buffer, size_t width, size_t height, double x, double y, Color color);
};