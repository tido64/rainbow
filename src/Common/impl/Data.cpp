// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Common/IO.h"

Data::Data(const char *const file, const Type type) :
	allocated(0), sz(0), data(nullptr)
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
		delete[] this->data;
		this->data = nullptr;
		this->allocated = 0;
	}
	else
		this->sz = size;
}

Data::~Data()
{
	delete[] this->data;
}

void Data::allocate(const size_t size)
{
	if (size >= this->allocated)  // Data needs a bigger buffer
	{
		delete[] this->data;
		this->allocated = size + 1;
		this->data = new unsigned char[this->allocated];
		this->data[size] = 0;
	}
	else  // Pad with zeros
		memset(this->data + size, 0, this->allocated - size);
}

void Data::copy(const void *const data, const size_t length)
{
	this->allocate(length);
	memcpy(this->data, data, length);
	this->sz = length;
}

bool Data::save(const char *const file) const
{
#ifdef RAINBOW_ANDROID
	static_cast<void>(file);
	return false;
#else
	R_ASSERT(this->data, "Can't save without destination");
	R_ASSERT(this->sz > 0, "Data is set but size is 0");

	Rainbow::IO::FileHandle fh =
			Rainbow::IO::find_and_open(file, Rainbow::IO::kIOTypeWrite);
	if (!fh)
		return false;

	// Write buffer to file
	const size_t written = Rainbow::IO::write(this->data, this->sz, fh);
	Rainbow::IO::close(fh);
	return written == this->sz;
#endif
}

void Data::set(unsigned char *data, const size_t length)
{
	delete[] this->data;
	this->data = data;
	this->allocated = length;
	this->sz = length;
}

#endif
