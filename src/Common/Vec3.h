#ifndef VEC3_H_
#define VEC3_H_

namespace Rainbow
{
	/// Structure for storing a three-dimensional vector.
	///
	/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct _Vec3
	{
		T x, y, z;

		_Vec3() : x(0), y(0), z(0) { }
		_Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) { }

		/// Determine whether the vector is zero.
		bool is_zero() const
		{
			return !this->x && !this->y && !this->z;
		}

		/// Zero the vector.
		void zero()
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}

		_Vec3<T>& operator+=(const _Vec3<T> &v)
		{
			this->x += v.x;
			this->y += v.y;
			this->z += v.z;
			return *this;
		}

		_Vec3<T>& operator-=(const _Vec3<T> &v)
		{
			this->x -= v.x;
			this->y -= v.y;
			this->z -= v.z;
			return *this;
		}

		_Vec3<T>& operator*=(const T &f)
		{
			this->x *= f;
			this->y *= f;
			this->z *= f;
			return *this;
		}
	};
}

typedef Rainbow::_Vec3<float> Vec3f;

#endif
