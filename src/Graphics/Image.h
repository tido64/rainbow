// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include "Common/DataMap.h"

namespace Rainbow
{
	struct Image
	{
		enum class Format
		{
			UNKNOWN,
			ATITC,  // Adreno
			ETC1,   // OpenGL ES standard
			PVRTC,  // iOS, OMAP43xx, PowerVR
			S3TC,   // Desktops, Tegra
			PNG
		};

		/// Creates an Image struct from image data.
		///
		/// Supports
		/// - iOS: PVRTC and whatever UIImage devours.
		/// - Other: PNG.
		///
		/// Limitations
		/// - PNG: Non-transparent grayscale is assumed to be PNG8, i.e. it's not supported.
		/// - PVRTC: PVR3 only, square, power of 2, no mipmaps, pre-multiplied alpha.
		static Image decode(const DataMap &);
		static void release(const Image &);

		Format format;
		unsigned int width;
		unsigned int height;
		unsigned int depth;
		unsigned int channels;
		size_t size;
		void *data;

		Image() :
			format(Format::UNKNOWN), width(0), height(0), depth(0), channels(0),
			size(0), data(nullptr) { }
	};
}

#include "Graphics/OpenGL.h"
#if defined(RAINBOW_OS_IOS)
#	include "Graphics/Decoders/UIKit.h"
#else
#	include "Graphics/Decoders/PNG.h"
#endif
#ifdef GL_IMG_texture_compression_pvrtc
#	include "Graphics/Decoders/PVRTC.h"
#endif  // GL_IMG_texture_compression_pvrtc

namespace Rainbow
{
	Image Image::decode(const DataMap &data)
	{
	#ifdef USE_PVRTC
		if (PVRTC::check(data))
			return PVRTC::decode(data);
	#endif  // USE_PVRTC

	#ifdef USE_PNG
		if (PNG::check(data))
			return PNG::decode(data);
	#endif  // USE_PNG

	#ifdef USE_UIKIT
		return UIKit::decode(data);
	#else
		return Image();
	#endif
	}

	void Image::release(const Image &image)
	{
		switch (image.format)
		{
			case Format::PVRTC:
				break;
			case Format::PNG:
			default:
				delete[] static_cast<unsigned char*>(image.data);
				break;
		}
	}
}

#endif
