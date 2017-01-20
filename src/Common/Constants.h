// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_CONSTANTS_H_
#define COMMON_CONSTANTS_H_

namespace rainbow
{
    // Mathematical constants

    template <typename T>
    constexpr auto kDegree = T(1.74532925199432957692369076848861271344287188854172545609719144017100911460344944368224156963450948221230449251e-02);

    template <typename T>
    constexpr auto kPi = T(3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651e+00);

    template <typename T>
    constexpr auto kPi_2 = T(1.57079632679489661923132169163975144209858469968755291048747229615390820314310449931401741267105853399107404326e+00);

    template <typename T>
    constexpr auto kRadian = T(5.72957795130823208767981548141051703324054724665643215491602438612028471483215526324409689958511109441862233816e+01);

    // Physical constants

    /// <summary>Standard gravitational acceleration value.</summary>
    constexpr float kStandardGravity = -9.80665f;

    // Other constants

    constexpr float kLowPassAlpha = 0.5f;
}

#endif
