---
id: math
title: Math (C++ only)
---

Small collection of common mathematical functions and convenient algorithms. All
of these functions are available under namespace `rainbow` unless otherwise
stated.

## Power of 2

```cpp
auto ceil_pow2(unsigned int i) -> unsigned int;
```

Rounds `i` up to the nearest power of 2.

> 0 is incorrectly considered a power of 2.

```cpp
auto floor_pow2(unsigned int i) -> unsigned int;
```

Rounds `i` down to the nearest power of 2.

```cpp
auto is_pow2(unsigned int i) -> bool;
```

Returns whether integer `i` is a power of 2.

## Trigonometry

```cpp
template <typename T>
auto degrees(T r) -> T;

template <typename T>
auto radian(T d) -> T;
```

Converts values between degrees and radians.

## Vec2

`Vec2` is a two-dimensional vector template whose value type must be arithmetic.
There are currently three predefined `Vec2` types in the global namespace.

```cpp
using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec2u = Vec2<unsigned>;
```

### Constants

```cpp
template <typename T>
const Vec2<T> Vec2<T>::Down;
```

Vector representing down; shorthand for `Vec2<T>(0, -1)`.

```cpp
template <typename T>
const Vec2<T> Vec2<T>::Left;
```

Vector representing left; shorthand for `Vec2<T>(-1, 0)`.

```cpp
template <typename T>
const Vec2<T> Vec2<T>::One;
```

One vector; shorthand for `Vec2<T>(1, 1)`.

```cpp
template <typename T>
const Vec2<T> Vec2<T>::Right;
```

Vector representing right; shorthand for `Vec2<T>(1, 0)`.

```cpp
template <typename T>
const Vec2<T> Vec2<T>::Up;
```

Vector representing up; shorthand for `Vec2<T>(0, 1)`.

```cpp
template <typename T>
const Vec2<T> Vec2<T>::Zero;
```

Zero vector; shorthand for `Vec2<T>(0, 0)`.

### Methods

```cpp
auto Vec2<T>::angle(const Vec2 &v) const -> float;
```

Returns the angle (in radians) between two points.

```cpp
auto Vec2<T>::distance(const Vec2 &v) const -> float;
```

Returns the distance between two points.

```cpp
auto Vec2<T>::distance_sq(const Vec2 &v) const -> float;
```

Returns the distance between two points, squared. This method returns before
calculating the square root as opposed to `distance()`. This can give you some
performance gains if you don't need the exact distance.

```cpp
auto Vec2<T>::is_zero() const -> bool;
```

Returns whether both vector components are zero.

### Operators

Assuming `v` and `w` are instances of `Vec2f`, Rainbow implements the following
operators:

#### Vector Negation

```cpp
Vec2f w = -v;
```

Equivalent to `w = Vec2f(-v.x, -v.y)`.

#### Vector Sum

```cpp
Vec2f u = v + w;
v += w;
```

Equivalent to `u = Vec2f(v.x + w.x, v.y + w.y)`.

#### Vector Difference

```cpp
Vec2f u = v - w;
v -= w;
```

Equivalent to `u = Vec2f(v.x - w.x, v.y - w.y)`.

#### Dot Product

```cpp
float f = v * w;
```

Equivalent to `f = v.x * w.x + v.y * w.y`.

#### Scalar Multiplication

```cpp
Vec2f u = 2.0f * v;
v *= 2.0f;
```

Equivalent to `u = Vec2f(2.0f * v.x, 2.0f * v.y)`.

#### Scalar Division

```cpp
Vec2f u = v / 2.0f;
v /= 2.0f;
```

Equivalent to `u = Vec2f(v.x / 2.0f, v.y / 2.0f)`.

#### Scalar Addition/Subtraction

```cpp
Vec2f u = v + 5.0f;
Vec2f w = v - 5.0f;
```

Equivalent to `u = Vec2f(v.x + 5.0f, v.y + 5.0f)`.

#### Comparison

```cpp
bool equal = v == w;
```

Equivalent to `v.x == w.x && v.y == w.y`.

## Miscellaneous

```cpp
template <typename T, typename = FloatingPoint<T>>
auto are_equal(T a, T b) -> bool;
```

Returns whether two floating point numbers are (almost) equal. Based on the
`AlmostEqualRelative()` implementation from a [Random ASCII blog post][1].

```cpp
auto fast_invsqrt(float x) -> float;
```

Returns an approximation of 1/√x using the infamous constant 0x5f3759df.

```cpp
auto is_almost_zero(float x) -> bool;
```

Returns whether `x` is practically zero. A number is considered almost zero if
it's within `10 * ε`. On some hardware, this is 0.0000011920929.

[1]: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/ "Comparing Floating Point Numbers, 2012 Edition"
