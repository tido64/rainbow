// Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include <cstring>

#include "Common/Data.h"
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

Data::Data(const File &file) : ownership(kDataOwner), allocated(0), sz(0), data(nullptr)
{
	if (!file)
		return;

	const size_t size = file.size();
	this->allocate(size);
	if (file.read(this->data, size) != size)
	{
		operator delete(this->data);
		this->data = nullptr;
		this->allocated = 0;
	}
	else
		this->sz = size;
}

Data::~Data()
{
	if (this->ownership != kDataOwner)
		return;

	operator delete(this->data);
}

bool Data::save(const char *const path) const
{
	R_ASSERT(this->data, "No data to save");
	R_ASSERT(this->sz > 0, "Data is set but size is 0");

	const File &file = File::open_write(path);
	if (!file)
		return false;

	return file.write(this->data, this->sz) == this->sz;
}

void Data::allocate(const size_t size)
{
	R_ASSERT(this->ownership == kDataOwner, "Cannot reallocate a read-only buffer");

	if (size >= this->allocated)
	{
		operator delete(this->data);
		this->allocated = size + 1;
		this->data = operator new(this->allocated);
		static_cast<char*>(this->data)[size] = 0;
	}
	else  // Pad with zeros.
		memset(static_cast<char*>(this->data) + size, 0, this->allocated - size);
}
