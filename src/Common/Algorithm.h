// Provides algorithms and mathematical methods.
//
// \see https://graphics.stanford.edu/~seander/bithacks.html
//
// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_ALGORITHM_H_
#define COMMON_ALGORITHM_H_

#include <algorithm>
#include <cmath>
#include <functional>
#include <iterator>
#include <limits>
#include <type_traits>
#include <utility>

#include "Common/Constants.h"
#include "Common/Constraints.h"

namespace rainbow
{
    /// <summary>
    ///   Compares two floating point numbers and approximates their equality.
    /// </summary>
    /// <returns><c>true</c> when approximately equal.</returns>
    template <typename T, typename = FloatingPoint<T>>
    bool are_equal(T a, T b)
    {
        const auto max = std::max(std::abs(a), std::abs(b));
        return std::abs(a - b) <= max * std::numeric_limits<T>::epsilon();
    }

    template <typename T, size_t N>
    constexpr auto array_size(const T (&)[N]) noexcept
    {
        return N;
    }

    /// <summary>Rounds up to the nearest power of 2.</summary>
    /// <remarks>Note that 0 is incorrectly considered a power of 2.</remarks>
    inline auto ceil_pow2(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return ++i;
    }

    /// <summary>Converts radians to degrees.</summary>
    template <typename T, typename = FloatingPoint<T>>
    constexpr auto degrees(T r)
    {
        return r * kRadian<T>;
    }

    /// <summary>Fast inverse square root by 0x5f3759df.</summary>
    inline auto fast_invsqrt(float x)
    {
        const float xhalf = x * 0.5f;
        union {
            uint32_t u;
            float f;
        };
        f = x;
        u = 0x5f3759df - (u >> 1);
        x = f;
        return x * (1.5f - (xhalf * x * x));
    }

    /// <summary>
    ///   Finds the first element equal to <paramref name="value"/>.
    /// </summary>
    template <typename Container, typename T>
    auto find(Container&& container, const T& value)
    {
        return std::find(std::begin(std::forward<Container>(container)),
                         std::end(std::forward<Container>(container)),
                         value);
    }

    /// <summary>Rounds down to the nearest power of 2.</summary>
    inline auto floor_pow2(unsigned int i)
    {
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i - (i >> 1);
    }

    template <typename T>
    void hash_combine(size_t& seed, T&& value)
    {
        const auto hash = std::hash<std::decay_t<T>>{}(value);
        seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    /// <summary>
    ///   Returns whether <paramref name="x"/> is practically zero. A number is
    ///   considered almost zero if it's within <c>10 * ε</c>. On some hardware,
    ///   this is 0.0000011920929.
    /// </summary>
    inline bool is_almost_zero(float x)
    {
        return std::abs(x) < std::numeric_limits<float>::epsilon() * 10.0f;
    }

    /// <summary>Determines whether an integer is a power of 2.</summary>
    constexpr bool is_pow2(unsigned int i)
    {
        return i && !(i & (i - 1));
    }

    /// <summary>Low-pass filter.</summary>
    inline auto low_pass(float value, float low_pass)
    {
        return kLowPassAlpha * std::pow(10.0f, value * kLowPassAlpha) +
               (1.0f - kLowPassAlpha) * low_pass;
    }

    constexpr auto make_fourcc(char a, char b, char c, char d)
    {
        return static_cast<uint32_t>(a) |  //
               (static_cast<uint32_t>(b) << 8) |
               (static_cast<uint32_t>(c) << 16) |
               (static_cast<uint32_t>(d) << 24);
    }

    /// <summary>
    ///   Removes the element at <paramref name="pos"/> by swapping it with the
    ///   last element in <paramref name="container"/> and popping it.
    /// </summary>
    template <typename T>
    void quick_erase(T& container, typename T::iterator pos)
    {
        std::swap(*pos, container.back());
        container.pop_back();
    }

    /// <summary>Converts degrees to radians.</summary>
    template <typename T, typename = FloatingPoint<T>>
    constexpr auto radians(T d)
    {
        return d * kDegree<T>;
    }

    /// <summary>
    ///   Removes the first element equal to <paramref name="value"/>.
    /// </summary>
    template <typename Container, typename T>
    auto remove(Container& container, const T& value)
    {
        auto i = find(container, value);
        return (i == std::end(container) ? i : container.erase(i));
    }

    /// <summary>
    ///   Removes all elements that satisfy <paramref name="predicate"/> and
    ///   returns a past-the-end iterator for the new end of the range.
    /// </summary>
    template <typename T, typename F>
    auto remove_if(T& container, F&& predicate)
    {
        return container.erase(std::remove_if(std::begin(container),
                                              std::end(container),
                                              std::forward<F>(predicate)),
                               std::end(container));
    }

    /// <summary>Performs a left rotation on the container.</summary>
    template <typename T>
    void rotate_left(T& container)
    {
        auto begin = std::begin(container);
        std::rotate(begin, begin + 1, std::end(container));
    }

    /// <summary>Performs a right rotation on the container.</summary>
    template <typename T>
    void rotate_right(T& container)
    {
        auto rbegin = std::rbegin(container);
        std::rotate(rbegin, rbegin + 1, std::rend(container));
    }

    /// <summary>
    ///   Returns -1, 0, or 1 for <paramref name="x"/> < 0,
    ///   <paramref name="x"/> == 0, or <paramref name="x"/> > 0 respectively.
    /// </summary>
    template <typename T>
    constexpr auto signum(T x)
    {
        return (x > T{}) - (x < T{});
    }

    template <typename Enum>
    constexpr auto to_integral_value(Enum value)
    {
        return static_cast<std::underlying_type_t<Enum>>(value);
    }
}

#endif
