// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_ACCELERATION_H_
#define INPUT_ACCELERATION_H_

#include "Math/Vec3.h"

namespace rainbow
{
    /// <summary>
    ///   Structure representing immediate, three-dimensional acceleration data.
    /// </summary>
    /// <remarks>
    ///   Implements a simple high-pass filter that removes the gravity
    ///   component from the acceleration data.
    /// </remarks>
    class Acceleration
    {
        static constexpr double kFilteringFactor = 0.1;

    public:
        [[nodiscard]] auto timestamp() const { return timestamp_; }
        [[nodiscard]] auto x() const { return pass_.x; }
        [[nodiscard]] auto y() const { return pass_.y; }
        [[nodiscard]] auto z() const { return pass_.z; }

        /// <summary>Updates acceleration data.</summary>
        /// <param name="x">Raw acceleration data (x-value).</param>
        /// <param name="y">Raw acceleration data (y-value).</param>
        /// <param name="z">Raw acceleration data (z-value).</param>
        /// <param name="t">
        ///   The relative time at which the acceleration event occurred.
        /// </param>
        void update(double x, double y, double z, double t)
        {
            // Calculate the low-pass value and subtract it from the measured
            // value.
            pass_.x = x - ((x * kFilteringFactor) +
                           (pass_.x * (1.0 - kFilteringFactor)));
            pass_.y = y - ((y * kFilteringFactor) +
                           (pass_.y * (1.0 - kFilteringFactor)));
            pass_.z = z - ((z * kFilteringFactor) +
                           (pass_.z * (1.0 - kFilteringFactor)));
            timestamp_ = t;
        }

    private:
        /// <summary>Filtered acceleration data.</summary>
        Vec3d pass_;

        /// <summary>
        ///   The relative time at which the acceleration event occurred.
        /// </summary>
        double timestamp_ = 0.0;
    };
}  // namespace rainbow

#endif
