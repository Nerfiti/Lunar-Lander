#pragma once

#include <cstdlib>

#include "Color.h"
#include "Landscape.h"

class Planet final
{
    public:
        Planet(size_t width, Color color = Color(200, 200, 200));
        void generate_landscape(size_t pixels_per_line, uint32_t height_mean, uint32_t height_std);

        void draw(uint32_t *buffer, size_t width, size_t height);

        bool check_collision(const RectCollider &collider) const;

    private:
        Landscape ground_;
        size_t width_;
        Color color_;
};