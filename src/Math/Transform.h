// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_TRANSFORM_H_
#define MATH_TRANSFORM_H_

#include "Math/Vec2.h"
#include "Memory/Array.h"

namespace rainbow
{
    template <typename Float>
    auto transform_srt(const Vec2<Float>& p,
                       const Vec2<Float>& s_sin_r,
                       const Vec2<Float>& s_cos_r,
                       const Vec2<Float>& position)
    {
        return Vec2<Float>{s_cos_r.x * p.x - s_sin_r.y * p.y + position.x,
                           s_sin_r.x * p.x + s_cos_r.y * p.y + position.y};
    }

    template <typename Float>
    auto transform_st(const Vec2<Float>& p,
                      const Vec2<Float>& scale,
                      const Vec2<Float>& position)
    {
        return Vec2<Float>{scale.x * p.x + position.x,   //
                           scale.y * p.y + position.y};
    }

    template <typename Float, typename Vertex>
    void transform(const Vec2<Float> (&quad)[4],
                   const Vec2<Float>& position,
                   const Vec2<Float>& scale,
                   ArraySpan<Vertex> data)
    {
        data[0].position = transform_st(quad[0], scale, position);
        data[1].position = transform_st(quad[1], scale, position);
        data[2].position = transform_st(quad[2], scale, position);
        data[3].position = transform_st(quad[3], scale, position);
    }

    template <typename Float, typename Vertex>
    void transform(const Vec2<Float> (&quad)[4],
                   const Vec2<Float>& position,
                   const Vec2<Float>& s_sin_r,
                   const Vec2<Float>& s_cos_r,
                   ArraySpan<Vertex> data)
    {
        data[0].position = transform_srt(quad[0], s_sin_r, s_cos_r, position);
        data[1].position = transform_srt(quad[1], s_sin_r, s_cos_r, position);
        data[2].position = transform_srt(quad[2], s_sin_r, s_cos_r, position);
        data[3].position = transform_srt(quad[3], s_sin_r, s_cos_r, position);
    }

    template <typename Float, typename Vertex>
    void transform(const Vec2<Float> (&quad)[4],
                   const Vec2<Float>& position,
                   Float angle,
                   const Vec2<Float>& scale,
                   ArraySpan<Vertex> data)
    {
        if (!is_almost_zero(angle))
        {
            transform(quad,
                      position,
                      std::sin(-angle) * scale,
                      std::cos(-angle) * scale,
                      data);
        }
        else
        {
            transform(quad, position, scale, data);
        }
    }

    template <typename T, typename Vertex>
    void transform(const T& sprite, ArraySpan<Vertex> data)
    {
        const float x0 = sprite.width() * -sprite.pivot().x;
        const float y0 = sprite.height() * (sprite.pivot().y - 1);
        const Vec2f quad[4]{
            {x0, y0},
            {x0 + sprite.width(), y0},
            {x0 + sprite.width(), y0 + sprite.height()},
            {x0, y0 + sprite.height()},
        };
        transform(
            quad, sprite.position(), sprite.angle(), sprite.scale(), data);
    }
}

#endif
