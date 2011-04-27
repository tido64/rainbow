#ifndef COLOR_H_
#define COLOR_H_

namespace Rainbow
{
	/// Structure for storing a color (RGBA).

	/// Copyright 2010 Bifrost Games. All rights reserved.
	/// \author Tommy Nguyen
	template<typename T>
	struct __Color
	{
		T r, g, b, a;

		__Color();
		__Color(const T r, const T g, const T b, const T a);
	};

	template<>
	struct __Color<unsigned char>
	{
		unsigned char r, g, b, a;

		__Color() : r(0xff), g(0xff), b(0xff), a(0xff) { }

		__Color(const unsigned char r, const unsigned char g, const unsigned char b, const unsigned char a = 0xff) :
			r(r), g(g), b(b), a(a)
		{ }
	};

	template<>
	struct __Color<float>
	{
		float r, g, b, a;

		__Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) { }

		__Color(const float r, const float g, const float b, const float a = 1.0f) :
			r(r), g(g), b(b), a(a)
		{ }
	};
}

typedef Rainbow::__Color<unsigned char> Colorb;
typedef Rainbow::__Color<float> Colorf;

#endif
