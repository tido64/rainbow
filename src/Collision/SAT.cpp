// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Collision/SAT.h"

#include <algorithm>

#include "Graphics/Sprite.h"

using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::Vec2f;

namespace
{
    struct Quad
    {
        Vec2f v0, v1, v2, v3;

        explicit Quad(const SpriteRef& sprite) : Quad(sprite->vertex_array()) {}

        explicit Quad(const SpriteVertex* vertices)
            : v0(vertices[0].position), v1(vertices[1].position),
              v2(vertices[2].position), v3(vertices[3].position)
        {
        }
    };

    template <typename T>
    bool overlaps(const std::pair<T, T>& a, const std::pair<T, T>& b)
    {
        return *a.first < *b.first ? *a.second >= *b.first
                                   : *a.first <= *b.second;
    }

    bool overlaps(const Quad& a, float ar, const Quad& b, float br)
    {
        int count = 4;
        Vec2f axes[8]{
            (a.v1 - a.v0).normal().normalize(),
            (a.v2 - a.v1).normal().normalize(),
            (a.v3 - a.v2).normal().normalize(),
            (a.v0 - a.v3).normal().normalize(),
        };
        if (!rainbow::are_equal(ar, br))
        {
            count = 8;
            axes[4] = (b.v1 - b.v0).normal().normalize();
            axes[5] = (b.v2 - b.v1).normal().normalize();
            axes[6] = (b.v3 - b.v2).normal().normalize();
            axes[7] = (b.v0 - b.v3).normal().normalize();
        }

        return std::all_of(axes, axes + count, [&a, &b](const Vec2f& axis) {
            const float a_dots[]{
                axis * a.v0, axis * a.v1, axis * a.v2, axis * a.v3};
            const float b_dots[]{
                axis * b.v0, axis * b.v1, axis * b.v2, axis * b.v3};
            return overlaps(
                std::minmax_element(std::begin(a_dots), std::end(a_dots)),
                std::minmax_element(std::begin(b_dots), std::end(b_dots)));
        });
    }
}  // namespace

bool rainbow::overlaps(const SpriteRef& a, const SpriteRef& b)
{
    return ::overlaps(Quad{a}, a->angle(), Quad{b}, b->angle());
}
