#include <cstring>
#include <png.h>

#include "Common/Debug.h"

#define USE_PNG

namespace PNG
{
	namespace
	{
		struct png_read_struct
		{
			size_t offset;
			const unsigned char *data;

			png_read_struct(const unsigned char *data) : offset(8), data(data) { }
		};

		void mem_fread(png_structp png_ptr, png_bytep data, png_size_t length)
		{
			png_read_struct *read_struct = static_cast<png_read_struct*>(png_get_io_ptr(png_ptr));
			memcpy(data, read_struct->data + read_struct->offset, length);
			read_struct->offset += length;
		}
	}

	bool check(const DataMap &data)
	{
		return png_sig_cmp(data, 0, 8) == 0;
	}

	Rainbow::Image decode(const DataMap &data)
	{
		Rainbow::Image image;
		image.format = Rainbow::Image::Format::PNG;

		// Prepare for decoding PNG data
		png_read_struct texture(data);

		png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
		if (!png_ptr)
		{
			R_ASSERT(png_ptr, "Failed to create PNG reading structure");
			return image;
		}

		png_infop info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
		{
			png_destroy_read_struct(&png_ptr, nullptr, nullptr);
			R_ASSERT(info_ptr, "Failed to initialize PNG information structure");
			return image;
		}

		if (setjmp(png_jmpbuf(png_ptr)))
		{
			png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
			R_ASSERT(false, "Failed to read PNG");
			return image;
		}

		// Offset to skip the signature
		png_set_sig_bytes(png_ptr, texture.offset);

		// Set reading function
		png_set_read_fn(png_ptr, &texture, mem_fread);

		// Retrieve PNG info
		png_read_info(png_ptr, info_ptr);
		const png_byte color_type = png_get_color_type(png_ptr, info_ptr);
		switch (color_type)
		{
			case PNG_COLOR_TYPE_GRAY:
				if (png_get_bit_depth(png_ptr, info_ptr) < 8)
					png_set_expand_gray_1_2_4_to_8(png_ptr);
				image.depth = 8;
				image.channels = 1;
				break;
			case PNG_COLOR_TYPE_PALETTE:
				png_set_palette_to_rgb(png_ptr);
			case PNG_COLOR_TYPE_RGB:
				image.depth = 24;
				image.channels = 3;
				break;
			case PNG_COLOR_TYPE_RGB_ALPHA:
				image.depth = 32;
				image.channels = 4;
				break;
			case PNG_COLOR_TYPE_GRAY_ALPHA:
				if (png_get_bit_depth(png_ptr, info_ptr) < 8)
					png_set_expand_gray_1_2_4_to_8(png_ptr);
				image.depth = 16;
				image.channels = 2;
				break;
			default:
				R_ERROR("Unknown PNG color type: %u\n", color_type);
				break;
		}

		image.width = png_get_image_width(png_ptr, info_ptr);
		image.height = png_get_image_height(png_ptr, info_ptr);
		R_ASSERT(image.width > 0 && image.height > 0,
		         "Invalid texture dimension");

		// Allocate memory for bitmap
		png_read_update_info(png_ptr, info_ptr);
		const size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
		image.data = new unsigned char[image.height * rowbytes];

		// Allocate row pointer array
		png_bytep *row_pointers = new png_bytep[image.height];

		png_byte *b = static_cast<png_byte*>(image.data);
		row_pointers[0] = b;
		for (size_t i = 1; i < image.height; ++i)
			row_pointers[i] = row_pointers[i - 1] + rowbytes;
		png_read_image(png_ptr, row_pointers);
		delete[] row_pointers;

		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

		return image;
	}
}
