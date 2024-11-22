#pragma once

#include "RectTexture.h"
#include "RectTransform.h"

class Sprite
{
    public:
        Sprite(const RectTexture &rect, bool expand = true);
        Sprite(RectTexture &&rect, bool expand = true);
        virtual ~Sprite() = default;

        RectTexture &get_rect();
        const RectTexture &get_rect() const;

        double get_sin_phi();
        double get_cos_phi();

        void set_center(int x, int y);
        void set_expand(bool expand);

        void move(double x, double y);
        void rotate(double phi);
        void draw(uint32_t *buffer, size_t width, size_t height);

    protected:
        RectTransform transform_;

    private:
        RectTexture rect_;
        bool expand_on_rotate_;

        void draw_pixel_with_interpolation(uint32_t *buffer, size_t width, size_t height, double x, double y, Color color);
};