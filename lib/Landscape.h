#pragma once

#include <cstdint>
#include <map>
#include <vector>

#include "RectCollider.h"

class Landscape final
{
    public:
        Landscape();

        void add_point(uint32_t x, uint32_t y);

        uint32_t get_height(uint32_t x);
        uint32_t get_height_naive(uint32_t x) const;

        bool check_collision(const RectCollider &collider, std::vector<CollisionInfo> &info) const;

    private:
        using ground_t = std::map<uint32_t, uint32_t>;
        ground_t ground_points_;

        mutable ground_t::iterator prev_point_;

        int64_t try_in_cache(uint32_t x) const;
        uint32_t interpolate(uint32_t x, uint32_t left, uint32_t right, uint32_t left_height, uint32_t right_height) const;
};