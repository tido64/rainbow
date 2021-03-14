/**
 * Copyright (C) 2016 Nishant Srivastava
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PLATFORM_ANDROID_SHAKEGESTUREDETECTOR_H_
#define PLATFORM_ANDROID_SHAKEGESTUREDETECTOR_H_

#include <functional>

#include "Common/Constants.h"
#include "Common/NonCopyable.h"
#include "Math/Vec3.h"

namespace rainbow
{
    class ShakeGestureDetector : private NonCopyable<ShakeGestureDetector>
    {
    public:
        template <typename F>
        ShakeGestureDetector(F&& on_shake)
            : ShakeGestureDetector(kStandardGravity * (-4.0F / 3.0F),
                                   300,
                                   std::forward<F>(on_shake))
        {
        }

        template <typename F>
        ShakeGestureDetector(float threshold,
                             int64_t min_shake_duration,
                             F&& on_shake)
            : threshold_(threshold), min_shake_duration_(min_shake_duration),
              on_shake_(std::forward<F>(on_shake))
        {
        }

        void update(Vec3f acceleration, int64_t timestamp)
        {
            const auto a = std::sqrt(acceleration.x * acceleration.x +
                                     acceleration.y * acceleration.y +
                                     acceleration.z * acceleration.z);
            const auto mag = magnitude_ * 0.9F + (a - acceleration_);
            if (mag > threshold_) {
                if (!is_shaking_)
                    on_shake_();

                is_shaking_ = true;
                last_shake_ = timestamp;
            } else if (is_shaking_ &&
                       timestamp - last_shake_ > min_shake_duration_) {
                is_shaking_ = false;
            }

            magnitude_ = mag;
            acceleration_ = a;
        }

    private:
        bool is_shaking_ = false;
        float magnitude_ = 0.0F;
        float acceleration_ = -kStandardGravity;
        float threshold_ = 0.0F;
        int64_t last_shake_ = 0;
        int64_t min_shake_duration_ = 0;
        std::function<void()> on_shake_;
    };
}  // namespace rainbow

#endif
