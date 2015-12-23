// Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef FILESYSTEM_FILE_H_
#define FILESYSTEM_FILE_H_

#include <cstdio>

#include "Common/NonCopyable.h"

struct AAsset;

class File : private NonCopyable<File>
{
public:
	static File open(const char* path);
	static File open_asset(const char* path);
	static File open_document(const char* path);
	static File open_write(const char* path);

	File() : is_asset_(false), stream_(nullptr) {}
	File(File&&);
	~File();

	/// <summary>Returns the file size.</summary>
	size_t size() const;

	/// <summary>
	///   Reads <paramref name="size"/> bytes from file into buffer
	///   <paramref name="dst"/>.
	/// </summary>
	/// <param name="dst">[out] Destination buffer.</param>
	/// <param name="size">Number of bytes to read.</param>
	/// <returns>Number of bytes read.</returns>
	size_t read(void* dst, size_t size) const;

	/// <summary>
	///   Sets the file position indicator for the file stream to the value
	///   pointed to by <paramref name="offset"/>.
	///   <seealso cref="fseek"/>
	/// </summary>
	/// <param name="offset">
	///   Number of bytes to shift the position relative to origin.
	/// </param>
	/// <param name="origin">Position to which offset is added.</param>
	/// <returns>0 upon success, nonzero value otherwise.</returns>
	int seek(long offset, int origin) const;

	/// <summary>Writes buffer at <paramref name="buffer"/> to file.</summary>
	/// <param name="buffer">Source buffer.</param>
	/// <param name="size">Number of bytes to write.</param>
	/// <returns>Number of bytes written.</returns>
	size_t write(const void* buffer, size_t size) const;

	explicit operator bool() const { return stream_; }
	operator AAsset*() const { return asset_; }
	operator FILE*() const { return stream_; }

private:
	bool is_asset_;  ///< Whether this file is an asset.

	union
	{
		AAsset* asset_;
		FILE* stream_;
	};

	explicit File(const char* path);
	File(const char* path, const char* mode);
};

#endif
