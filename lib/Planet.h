#pragma once

#include <cstdlib>

#include "Color.h"
#include "Landscape.h"

class Planet final
{
    public:
        Planet(size_t width, size_t height, Color color = Color(200, 200, 200));
        void generate_landscape(size_t pixels_per_line, uint32_t height_mean, uint32_t height_std);

        void generate_stars();

        void draw(uint32_t *buffer, size_t width, size_t height);

        bool check_collision(const RectCollider &collider, std::vector<CollisionInfo> &info, float dt) const;

    private:
        Landscape ground_;
        size_t width_;
        size_t height_;
        Color color_;

        static constexpr size_t Stars_count = 100;
        std::array<Vector2d, Stars_count> stars_;

        int32_t generate_rand_from_to(int32_t from, int32_t to) const;
};