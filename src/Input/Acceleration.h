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
	inline Acceleration();

	inline double timestamp() const;
	inline double x() const;
	inline double y() const;
	inline double z() const;

	/// Updates acceleration data.
	/// \param x,y,z  Raw acceleration data.
	/// \param t      The relative time at which the acceleration event occurred.
	inline void update(const double x, const double y, const double z, const double t);

private:
	Vec3d pass_;        ///< Filtered acceleration data.
	double timestamp_;  ///< The relative time at which the acceleration event occurred.
};

Acceleration::Acceleration() : timestamp_(0.0) { }

double Acceleration::timestamp() const
{
	return timestamp_;
}

double Acceleration::x() const
{
	return pass_.x;
}

double Acceleration::y() const
{
	return pass_.y;
}

double Acceleration::z() const
{
	return pass_.z;
}

void Acceleration::update(const double x, const double y, const double z, const double t)
{
	// Calculate the low-pass value and subtract it from the measured value.
	pass_.x = x - ((x * kFilteringFactor) + (pass_.x * (1.0 - kFilteringFactor)));
	pass_.y = y - ((y * kFilteringFactor) + (pass_.y * (1.0 - kFilteringFactor)));
	pass_.z = z - ((z * kFilteringFactor) + (pass_.z * (1.0 - kFilteringFactor)));
	timestamp_ = t;
}

#endif
