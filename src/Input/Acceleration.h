// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef INPUT_ACCELERATION_H_
#define INPUT_ACCELERATION_H_

#include "Common/Vec3.h"

#define kFilteringFactor 0.1

/// Structure representing immediate, three-dimensional acceleration data.
///
/// Implements a simple high-pass filter that removes the gravity component from
/// the acceleration data.
class Acceleration
{
public:
	double timestamp;         ///< The relative time at which the acceleration event occurred.
	const double &x, &y, &z;  ///< Filtered acceleration data.

	inline Acceleration();

	/// Updates acceleration data.
	/// \param x,y,z  Raw acceleration data.
	/// \param t      The relative time at which the acceleration event occurred.
	inline void update(const double x, const double y, const double z, const double t);

private:
	Vec3d pass_;  ///< Filtered acceleration data.
};

Acceleration::Acceleration()
    : timestamp(0.0), x(pass_.x), y(pass_.y), z(pass_.z) { }

void Acceleration::update(const double x, const double y, const double z, const double t)
{
	// Calculate the low-pass value and subtract it from the measured value.
	pass_.x = x - ((x * kFilteringFactor) + (pass_.x * (1.0 - kFilteringFactor)));
	pass_.y = y - ((y * kFilteringFactor) + (pass_.y * (1.0 - kFilteringFactor)));
	pass_.z = z - ((z * kFilteringFactor) + (pass_.z * (1.0 - kFilteringFactor)));
	timestamp = t;
}

#endif
