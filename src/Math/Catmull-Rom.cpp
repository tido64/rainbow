// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Math/Catmull-Rom.h"

#include <array>
#include <cmath>
#include <tuple>

namespace cr = rainbow::catmull_rom;

namespace
{
    using rg3f = std::array<float, 3>;
    using rg4f = std::array<float, 4>;
    using rg4Vec2f = std::array<Vec2f, 4>;

    auto cr_derivative(const rg4Vec2f& c, float t)
    {
        const float tt = t * t;
        return Vec2f{3.0f * c[3].x * tt + 2.0f * c[2].x * t + c[1].x,
                     3.0f * c[3].y * tt + 2.0f * c[2].y * t + c[1].y};
    }

    auto cr_solve(const rg4Vec2f& c, float t)
    {
        const float tt = t * t;
        const float ttt = tt * t;
        return Vec2f{c[3].x * ttt + c[2].x * tt + c[1].x * t + c[0].x,
                     c[3].y * ttt + c[2].y * tt + c[1].y * t + c[0].y};
    }

    auto nonuniform_parameterization(const rg4f& x, const rg3f& dt)
    {
        const float t1 =
            ((x[1] - x[0]) / dt[0] - (x[2] - x[0]) / (dt[0] + dt[1]) +
             (x[2] - x[1]) / dt[1]) *
            dt[1];
        const float t2 =
            ((x[2] - x[1]) / dt[1] - (x[3] - x[1]) / (dt[1] + dt[2]) +
             (x[3] - x[2]) / dt[2]) *
            dt[1];
        return rg4f{{x[1],
                     t1,
                     -3.0f * x[1] + 3.0f * x[2] - 2.0f * t1 - t2,
                     2.0f * x[1] - 2.0f * x[2] + t1 + t2}};
    }

    template <typename F, typename G>
    auto solve_with(const ArrayView<Vec2f>& points,
                    float t,
                    F&& parameterization,
                    G&& solve)
    {
        R_ASSERT(points.size() >= 4,
                 "Catmull–Rom spline requires at least four control points");
        R_ASSERT(t >= 0.0f && t <= 1.0f, "Invalid value for parameter 't'");

        const float u = t * (points.size() - 3);
        const float segment = std::trunc(u);
        return solve(
            parameterization(points.data() + static_cast<uint32_t>(segment)),
            u - segment);
    }

    auto unzip4(const Vec2f* p)
    {
        return std::make_tuple(rg4f{{p[0].x, p[1].x, p[2].x, p[3].x}},
                               rg4f{{p[0].y, p[1].y, p[2].y, p[3].y}});
    }

    auto zip(const rg4f& x, const rg4f& y) -> rg4Vec2f
    {
        return {{{x[0], y[0]}, {x[1], y[1]}, {x[2], y[2]}, {x[3], y[3]}}};
    }

    auto centripetal_parameterization(const Vec2f* points)
    {
        rg3f dt{{std::sqrt(points[0].distance(points[1])),
                 std::sqrt(points[1].distance(points[2])),
                 std::sqrt(points[2].distance(points[3]))}};

        if (rainbow::is_almost_zero(dt[1]))
            dt[1] = 1.0f;
        if (rainbow::is_almost_zero(dt[0]))
            dt[0] = dt[1];
        if (rainbow::is_almost_zero(dt[2]))
            dt[2] = dt[1];

        const auto xy = unzip4(points);
        return zip(nonuniform_parameterization(std::get<0>(xy), dt),
                   nonuniform_parameterization(std::get<1>(xy), dt));
    }
}

auto cr::derivative(const ArrayView<Vec2f>& points, float t) -> Vec2f
{
    return solve_with(points, t, centripetal_parameterization, cr_derivative);
}

auto cr::solve(const ArrayView<Vec2f>& points, float t) -> Vec2f
{
    return solve_with(points, t, centripetal_parameterization, cr_solve);
}
