#ifndef VEC3_H_
#define VEC3_H_

namespace Rainbow
{
	/// Structure for storing a three-dimensional vector.

	/// Copyright 2010 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct __Vec3
	{
		T x, y, z;

		__Vec3() : x(0), y(0), z(0) { }
		__Vec3(const T x, const T y, const T z) : x(x), y(y), z(z) { }

		/// Zero the vector.
		void zero()
		{
			this->x = 0;
			this->y = 0;
			this->z = 0;
		}
	};
}

typedef Rainbow::__Vec3<float> Vec3f;

#endif
