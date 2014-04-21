// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>
#include <png.h>

#include "Common/Debug.h"
#include "Common/Functional.h"

#define USE_PNG

namespace PNG
{
	bool check(const DataMap &data) pure;
	Rainbow::Image decode(const DataMap &data) pure;

	bool check(const DataMap &data)
	{
		return png_sig_cmp(data, 0, 8) == 0;
	}

	Rainbow::Image decode(const DataMap &data)
	{
		Rainbow::Image image;
		image.format = Rainbow::Image::Format::PNG;

		png_image pi;
		memset(&pi, 0, sizeof(pi));
		pi.version = PNG_IMAGE_VERSION;

		if (!png_image_begin_read_from_memory(&pi, data, data.size()))
			return image;

		image.width = pi.width;
		image.height = pi.height;
		if (PNG_IMAGE_PIXEL_CHANNELS(pi.format) == 2)
			pi.format = PNG_FORMAT_GA;
		else
			pi.format = PNG_FORMAT_RGBA;
		image.depth = PNG_IMAGE_SAMPLE_SIZE(pi.format) * 8;
		image.channels = PNG_IMAGE_SAMPLE_CHANNELS(pi.format);
		image.data = new unsigned char[PNG_IMAGE_SIZE(pi)];
		png_image_finish_read(
		    &pi, nullptr, image.data, PNG_IMAGE_ROW_STRIDE(pi), nullptr);
		return image;
	}
}
