#ifndef IMAGELOADER_H_
#define IMAGELOADER_H_

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Graphics/OpenGL.h"
#if defined(RAINBOW_IOS)
#	include <UIKit/UIKit.h>
#else
#	include <png.h>

/// Structure for reading PNG bitmaps.
///
/// Copyright 2010-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen
struct png_read_struct
{
	size_t offset;
	const unsigned char *data;

	png_read_struct(unsigned char *data) : offset(8), data(data) { }
};

static void mem_fread(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_read_struct *read_struct = static_cast<png_read_struct*>(png_get_io_ptr(png_ptr));
	memcpy(data, read_struct->data + read_struct->offset, length);
	read_struct->offset += length;
}

#endif

#ifdef GL_IMG_texture_compression_pvrtc

/// PVRTC texture header, as specified by Imagination Technologies Ltd.
/// \see http://www.imgtec.net/powervr/insider/docs/PVR%20File%20Format.Specification.1.0.11.External.pdf
struct PVRTexHeader
{
	uint32_t version;
	uint32_t flags;
	uint64_t pixel_format;
	uint32_t colour_space;
	uint32_t channel_type;
	uint32_t height;
	uint32_t width;
	uint32_t depth;
	uint32_t num_surfaces;
	uint32_t num_faces;
	uint32_t mipmap_count;
	uint32_t metadata_size;
};

#endif

struct ImageInfo
{
	unsigned int width;
	unsigned int height;
	unsigned int depth;
	unsigned int channels;
	unsigned int compressed;
	size_t size;
};

struct ImageLoader
{
	enum ImageFormat
	{
		UNKNOWN,
		ATITC,  // Adreno
		ETC1,   // OpenGL ES standard, Android
		PVRTC,  // iOS, OMAP43xx
		S3TC,   // Desktops, Tegra
		PNG,
		BMP
	};

	/// Attempt to retrieve the image format.
	/// \param img Encoded image data.
	/// \return Image format; \c ImageFormat::UNKNOWN if unknown.
	static ImageFormat get_format(const Data &img)
	{
	#ifdef GL_IMG_texture_compression_pvrtc
		if (*(uint32_t*)img.bytes() == 0x03525650)
			return PVRTC;
	#endif

	#ifdef PNG_LIBPNG_VER_STRING
		if (png_sig_cmp(img, 0, 8) == 0)
			return PNG;
	#endif

		return UNKNOWN;
	}

	/// Decode image data.
	///
	/// Supports
	/// - iOS: PVRTC and whatever UIImage devours.
	/// - Other: PNG.
	///
	/// Limitations
	/// - PNG: Non-transparent grayscale is assumed to be PNG8, i.e. it's not supported.
	/// - PVRTC: PVR3 only, square, power of 2, no mipmaps, pre-multiplied alpha.
	///
	/// \param[out] data        Output data array. Can be passed directly to
	///                         renderer. Caller is responsible for cleaning up
	///                         the array using \c ImageLoader::release().
	/// \param[out] image_info  Struct containing information regarding the deocded image.
	/// \param[in]  img         Encoded image data.
	/// \return \c true on success, \c false otherwise.
	static bool load(void *&data, ImageInfo &image_info, const Data &img)
	{
		memset(&image_info, 0, sizeof(image_info));
		switch (get_format(img))
		{
			case PVRTC:
				image_info.compressed = PVRTC;
				#ifdef RAINBOW_IOS
				{
					PVRTexHeader *header = (PVRTexHeader*)img.bytes();
					R_ASSERT(CFSwapInt32LittleToHost(header->mipmap_count) == 1, "Mipmaps are not supported yet");

					image_info.height = CFSwapInt32LittleToHost(header->height);
					image_info.width = CFSwapInt32LittleToHost(header->width);
					R_ASSERT(image_info.width == image_info.height, "Compressed textures *must* be square");

					const uint64_t format = CFSwapInt32LittleToHost(header->pixel_format);
					R_ASSERT(format < 4, "Invalid PVRTC format type");
					image_info.depth = (format < 2) ? 2 : 4;
					image_info.channels = ((format & 0x01) == 0) ? 3 : 4;

					image_info.size = image_info.width * image_info.height * image_info.depth >> 3;
					const size_t header_size = sizeof(*header) + CFSwapInt32LittleToHost(header->metadata_size);
					R_ASSERT(header_size + image_info.size == img.size(), "Unsupported PVR file format");

					unsigned char *bytes = new unsigned char[image_info.size];
					memcpy(bytes, img.bytes() + header_size, image_info.size);
					data = bytes;
				}
				#endif
				break;
			case PNG:
				#ifdef PNG_LIBPNG_VER_STRING
				{
					// Prepare for decoding PNG data
					png_read_struct texture(img);

					png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
					if (!png_ptr)
					{
						R_ASSERT(png_ptr, "Failed to create PNG reading structure");
						return false;
					}

					png_infop info_ptr = png_create_info_struct(png_ptr);
					if (!info_ptr)
					{
						png_destroy_read_struct(&png_ptr, nullptr, nullptr);
						R_ASSERT(info_ptr, "Failed to initialize PNG information structure");
						return false;
					}

					if (setjmp(png_jmpbuf(png_ptr)))
					{
						png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
						R_ASSERT(false, "Failed to read PNG");
						return false;
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
							image_info.depth = 8;
							image_info.channels = 1;
							break;
						case PNG_COLOR_TYPE_PALETTE:
							png_set_palette_to_rgb(png_ptr);
						case PNG_COLOR_TYPE_RGB:
							image_info.depth = 24;
							image_info.channels = 3;
							break;
						case PNG_COLOR_TYPE_RGB_ALPHA:
							image_info.depth = 32;
							image_info.channels = 4;
							break;
						case PNG_COLOR_TYPE_GRAY_ALPHA:
							if (png_get_bit_depth(png_ptr, info_ptr) < 8)
								png_set_expand_gray_1_2_4_to_8(png_ptr);
							image_info.depth = 16;
							image_info.channels = 2;
							break;
						default:
							R_ERROR("Unknown PNG color type: %u\n", color_type);
							break;
					}

					image_info.width = png_get_image_width(png_ptr, info_ptr);
					image_info.height = png_get_image_height(png_ptr, info_ptr);
					R_ASSERT(image_info.width > 0 && image_info.height > 0, "Invalid texture dimension");

					// Allocate memory for bitmap
					png_read_update_info(png_ptr, info_ptr);
					const size_t rowbytes = png_get_rowbytes(png_ptr, info_ptr);
					data = new unsigned char[image_info.height * rowbytes];

					// Allocate row pointer array
					png_bytep *row_pointers = new png_bytep[image_info.height];

					png_byte *b = static_cast<png_byte*>(data);
					row_pointers[0] = b;
					for (size_t i = 1; i < image_info.height; ++i)
						row_pointers[i] = row_pointers[i - 1] + rowbytes;
					png_read_image(png_ptr, row_pointers);
					delete[] row_pointers;

					png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
				}
				#endif
				break;
			default:
				#ifdef RAINBOW_IOS
				{
					UIImage *image = [UIImage imageWithData:img];
					if (!image)
					{
						R_ASSERT(image, "Unknown texture format");
						return false;
					}

					image_info.width = CGImageGetWidth(image.CGImage);
					image_info.height = CGImageGetHeight(image.CGImage);
					image_info.depth = CGImageGetBitsPerPixel(image.CGImage);
					image_info.channels = image_info.depth / CGImageGetBitsPerComponent(image.CGImage);
					if (image_info.depth == 24 || (image_info.depth == 8 && image_info.channels == 1))
					{
						// - Increase depth from 24 to 32 because iOS breaks on GL_RGB; or
						// - Assume PNG8.
						image_info.depth = 32;
						image_info.channels = 4;
					}
					CGRect bounds = CGRectMake(0, 0, image_info.width, image_info.height);

					CGColorSpaceRef color_space = CGColorSpaceCreateDeviceRGB();
					data = new unsigned char[image_info.height * image_info.width * 4];

					CGContextRef context = CGBitmapContextCreate(data, image_info.width, image_info.height, 8, 4 * image_info.width, color_space, kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big);
					CGColorSpaceRelease(color_space);

					CGContextClearRect(context, bounds);
					CGContextTranslateCTM(context, 0, 0);
					CGContextDrawImage(context, bounds, image.CGImage);
					CGContextRelease(context);

					image = nil;
				}
				#endif
				break;
		}
		return true;
	}

	static inline void release(void *data)
	{
		delete[] static_cast<unsigned char*>(data);
	}
};

#endif
