// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef MATH_CATMULL_ROM_H_
#define MATH_CATMULL_ROM_H_

#include "Math/Vec2.h"
#include "Memory/Array.h"

namespace rainbow { namespace catmull_rom
{
    /// <summary>Returns the derivative of the spline at given time.</summary>
    /// <param name="points">Control points of the spline.</param>
    /// <param name="t">The time in range [0.0, 1.0].</param>
    /// <remarks><see cref="solve"/>.</remarks>
    auto derivative(const ArrayView<Vec2f>& points, float t) -> Vec2f;

    /// <summary>Returns the point on the spline at given time.</summary>
    /// <param name="points">Control points of the spline.</param>
    /// <param name="t">The time in range [0.0, 1.0].</param>
    /// <remarks>
    ///   A Catmull-Rom spline segment A -> B is calculated using control points
    ///   preceding and succeeding the relevant ones. Meaning at least four
    ///   control points are required for a valid spline. The first and last
    ///   control points do not directly contribute to the spline's length.
    /// </remarks>
    auto solve(const ArrayView<Vec2f>& points, float t) -> Vec2f;
}}  // rainbow::catmull_rom

#endif
