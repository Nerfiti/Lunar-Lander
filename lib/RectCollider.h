#pragma once

#include <array>
#include <cstddef>

#include "Collider.h"
#include "RectTransform.h"

class RectCollider final : public Collider
{
    public:
        static constexpr size_t Number_of_vertices = 4;
        using polygon_t = std::array<Vector2d, Number_of_vertices>;

        RectCollider(Vector2d size, Vector2d position = Vector2d(), Vector2d pivot = Vector2d(), double angle = 0.0);

        virtual bool check_collision(const Segment& other) const override;
        virtual bool check_collision(const RectCollider& other) const override;

        void move(double x, double y);
        void move(Vector2d offset);

        void rotate(double angle);

        void set_position(Vector2d position);
        void set_angle(double angle);
        void set_pivot(Vector2d pivot);

        const RectTransform &get_transform() const;

        const polygon_t &get_vertices() const;

    private:
        RectTransform transform_;

        void update_AABB();

        template<size_t first_vertices, size_t second_vertices>
        bool SAT_check(const std::array<Vector2d, first_vertices> &lhs,
                       const std::array<Vector2d, second_vertices> &rhs) const;

        template<size_t first_vertices, size_t second_vertices>
        bool SAT_check_only_first_edges(const std::array<Vector2d,  first_vertices> &lhs,
                                        const std::array<Vector2d, second_vertices> &rhs) const;

        template<size_t first_vertices, size_t second_vertices>
        bool SAT_check_one_axis(const std::array<Vector2d,  first_vertices> &lhs,
                                const std::array<Vector2d, second_vertices> &rhs,
                                Vector2d target_axis) const;

        template<size_t number_of_vertices>
        Vector2d get_min_max_projection_coord(const std::array<Vector2d, number_of_vertices> &array, Vector2d target_axis) const;


        mutable bool need_update_vertices;
        mutable polygon_t transformed_vertices_;
        void update_vertices() const;
};