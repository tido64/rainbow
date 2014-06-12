// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"

#include <cstring>

#include "Common/Debug.h"
#include "FileSystem/File.h"

Data Data::load_asset(const char *const asset)
{
	return Data(File::open_asset(asset));
}

Data Data::load_document(const char *const document)
{
	return Data(File::open_document(document));
}

Data::Data(const File &file)
    : ownership_(kDataOwner), allocated_(0), sz_(0), data_(nullptr)
{
	if (!file)
		return;

	const size_t size = file.size();
	allocate(size);
	if (file.read(data_, size) != size)
	{
		operator delete(data_);
		data_ = nullptr;
		allocated_ = 0;
	}
	else
		sz_ = size;
}

Data::~Data()
{
	if (ownership_ != kDataOwner)
		return;

	operator delete(data_);
}

bool Data::save(const char *const path) const
{
	R_ASSERT(data_, "No data to save");
	R_ASSERT(sz_ > 0, "Data is set but size is 0");

	const File &file = File::open_write(path);
	if (!file)
		return false;

	return file.write(data_, sz_) == sz_;
}

void Data::allocate(const size_t size)
{
	R_ASSERT(ownership_ == kDataOwner, "Cannot reallocate a read-only buffer");

	if (size >= allocated_)
	{
		operator delete(data_);
		allocated_ = size + 1;
		data_ = operator new(allocated_);
		static_cast<char*>(data_)[size] = 0;
	}
	else  // Pad with zeros.
		memset(static_cast<char*>(data_) + size, 0, allocated_ - size);
}
