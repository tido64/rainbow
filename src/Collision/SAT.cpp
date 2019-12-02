// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Collision/SAT.h"

#include <algorithm>
#include <array>

#include "Graphics/Sprite.h"

using rainbow::SpriteRef;
using rainbow::SpriteVertex;
using rainbow::Vec2f;

namespace
{
    struct Quad
    {
        // NOLINTNEXTLINE(misc-non-private-member-variables-in-classes)
        Vec2f v0, v1, v2, v3;

        explicit Quad(const SpriteRef& sprite) : Quad(sprite->vertex_array()) {}

        explicit Quad(const SpriteVertex* vertices)
            : v0(vertices[0].position), v1(vertices[1].position),
              v2(vertices[2].position), v3(vertices[3].position)
        {
        }
    };

    template <typename T>
    auto overlaps(const std::pair<T, T>& a, const std::pair<T, T>& b) -> bool
    {
        return *a.first < *b.first ? *a.second >= *b.first
                                   : *a.first <= *b.second;
    }

    auto overlaps(const Quad& a, float ar, const Quad& b, float br) -> bool
    {
        int count = 4;
        std::array<Vec2f, 8> axes{
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

        return std::all_of(
            std::begin(axes),
            std::begin(axes) + count,
            [&a, &b](const Vec2f& axis) {
                const std::array<float, 4> a_dots{
                    axis * a.v0, axis * a.v1, axis * a.v2, axis * a.v3};
                const std::array<float, 4> b_dots{
                    axis * b.v0, axis * b.v1, axis * b.v2, axis * b.v3};
                return overlaps(
                    std::minmax_element(std::begin(a_dots), std::end(a_dots)),
                    std::minmax_element(std::begin(b_dots), std::end(b_dots)));
            });
    }
}  // namespace

auto rainbow::overlaps(const SpriteRef& a, const SpriteRef& b) -> bool
{
    return ::overlaps(Quad{a}, a->angle(), Quad{b}, b->angle());
}
