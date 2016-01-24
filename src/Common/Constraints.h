// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_CONSTRAINTS_H_
#define COMMON_CONSTRAINTS_H_

#include <type_traits>

template <typename T, typename U>
using EnableIfBaseOf =
    typename std::enable_if<std::is_base_of<T, U>::value>::type;

template <typename T, typename U = T>
using EnableIfFloatingPoint =
    typename std::enable_if<std::is_floating_point<T>::value, U>::type;

template <typename T, typename U = T>
using EnableIfIntegral =
    typename std::enable_if<std::is_integral<T>::value, U>::type;

template <typename T>
using Arithmetic = typename std::enable_if<std::is_arithmetic<T>::value>::type;

template <typename T>
using CharType =
    typename std::enable_if<std::is_same<typename std::make_unsigned<T>::type,
                                         unsigned char>::value>::type;

template <typename T>
using FloatingPoint = EnableIfFloatingPoint<T>;

template <typename T>
using PlainType =
    typename std::remove_pointer<typename std::decay<T>::type>::type;

#endif
