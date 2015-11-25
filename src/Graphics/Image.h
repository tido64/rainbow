// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef GRAPHICS_IMAGE_H_
#define GRAPHICS_IMAGE_H_

#include "Common/DataMap.h"

namespace rainbow
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

		/// <summary>Creates an Image struct from image data.</summary>
		/// <remarks>
		///   Supports
		///   <list type="bullet">
		///     <item>iOS: PVRTC and whatever UIImage devours.</item>
		///     <item>Other: PNG.</item>
		///   </list>
		///   Limitations
		///   <list type="bullet">
		///     <item>
		///       PNG: Non-transparent grayscale is assumed to be PNG8, i.e.
		///       it's not supported.
		///     </item>
		///     <item>
		///       PVRTC: PVR3 only, square, power of 2, no mipmaps,
		///       pre-multiplied alpha.
		///     </item>
		///   </list>
		/// </remarks>
		static Image decode(const DataMap&);
		static void release(const Image&);

		Format format;
		unsigned int width;
		unsigned int height;
		unsigned int depth;
		unsigned int channels;
		size_t size;
		const unsigned char* data;

		Image()
		    : format(Format::UNKNOWN), width(0), height(0), depth(0),
		      channels(0), size(0), data(nullptr) {}
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

namespace rainbow
{
	Image Image::decode(const DataMap& data)
	{
#ifdef USE_PVRTC
		if (pvrtc::check(data))
			return pvrtc::decode(data);
#endif  // USE_PVRTC

#ifdef USE_PNG
		if (png::check(data))
			return png::decode(data);
#endif  // USE_PNG

#ifdef USE_UIKIT
		return uikit::decode(data);
#else
		return Image();
#endif
	}

	void Image::release(const Image& image)
	{
		switch (image.format)
		{
			case Format::PVRTC:
				break;
			case Format::PNG:
			default:
				delete[] image.data;
				break;
		}
	}
}

#endif
