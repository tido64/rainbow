/// Copyright 2010-13 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#ifndef COMMON_CONSTANTS_H_
#define COMMON_CONSTANTS_H_

// Mathematical constants
const float kPi            = 3.14159265f;
const float kPi_2          = kPi / 2;
const float kDegree        = kPi / 180;
const float kLowPassAlpha  = 0.5f;
const float kRadian        = 180 / kPi;

// Physical constants
const float kStandardGravity = -9.80665f;  ///< Standard gravitational acceleration value

// Platform constants
const int kMaxInt = static_cast<unsigned int>(-1) >> 1;

#endif
