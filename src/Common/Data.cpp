// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Common/IO.h"

Data::Data(const char *const file, const Type type) :
	ownership(kDataOwner), allocated(0), sz(0), data(nullptr)
{
	Rainbow::IO::FileHandle fh =
			Rainbow::IO::find_and_open(file, static_cast<Rainbow::IO::Type>(type));
	if (!fh)
	{
		R_ERROR("[Rainbow] Data: Failed to open '%s'\n", file);
		return;
	}

	const size_t size = Rainbow::IO::size(fh);
	this->allocate(size);

	const size_t read = Rainbow::IO::read(this->data, size, fh);
	Rainbow::IO::close(fh);

	if (read != size)
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

bool Data::save(const char *const file) const
{
	R_ASSERT(this->data, "No data to save");
	R_ASSERT(this->sz > 0, "Data is set but size is 0");

	Rainbow::IO::FileHandle fh =
			Rainbow::IO::find_and_open(file, Rainbow::IO::kIOTypeWrite);
	if (!fh)
		return false;

	const size_t written = Rainbow::IO::write(this->data, this->sz, fh);
	Rainbow::IO::close(fh);
	return written == this->sz;
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
