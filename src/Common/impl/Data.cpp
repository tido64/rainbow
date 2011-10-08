/// Copyright 2011 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#include "Common/Data.h"

#if defined(RAINBOW_IOS)

void Data::free(const void *const p) { /* This is not needed */ }

const char* Data::get_path(const char *const file, const bool user_data)
{
	if (user_data)
	{
		// return user data path
		assert(!"Data::get_path: Not implemented yet");
		return 0;
	}

	if (file == 0)
		return [[[NSBundle mainBundle] bundlePath] UTF8String];

	NSString *path = [NSString stringWithUTF8String:(file)];
	return [[[NSBundle mainBundle] pathForResource:[path stringByDeletingPathExtension] ofType:[path pathExtension]] UTF8String];
}

Data::Data() : data(nil) { }

Data::Data(const char *const file) : data(nil)
{
	this->load(file);
}

Data::~Data()
{
	[data release];
}

bool Data::copy(const void *const data, const unsigned int length)
{
	assert(!"Data::copy: Not implemented yet");
	return true;
}

bool Data::load(const char *const file)
{
	NSError *err = nil;
	NSString *path = [NSString stringWithUTF8String:file];
	this->data = [[NSMutableData alloc] initWithContentsOfFile:path options:NSDataReadingUncached error:&err];
	if (err != nil)
	{
		[data release];
		NSLog(@"Rainbow::Data: Failed to read file");
		this->data = nil;
		return false;
	}
	return true;
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	assert(!"Data::save: Not implemented yet");

	return true;
}

#elif defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)

#include <cassert>
#include <cstdio>
#include <cstring>

void Data::free(const void *const p)
{
	delete[] static_cast<const char *const>(p);
}

const char* Data::get_path(const char *const file, const bool user_data)
{
	if (file == 0)
		return (user_data) ? RAINBOW_DATA_USER_PATH : RAINBOW_DATA_ASSETS_PATH;

	char *path = new char[strlen(file) + ((user_data) ? RAINBOW_DATA_USER_PATH_LENGTH : RAINBOW_DATA_ASSETS_PATH_LENGTH)];
	strcpy(path, (user_data) ? RAINBOW_DATA_USER_PATH : RAINBOW_DATA_ASSETS_PATH);
	strcat(path, file);
	return path;
}

Data::Data() : allocated(0), sz(0), data(nullptr) { }

Data::Data(const char *const file) : allocated(0), sz(0), data(nullptr)
{
	this->load(file);
}

Data::~Data()
{
	delete[] this->data;
}

bool Data::allocate(const unsigned int size)
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

	return true;
}

bool Data::copy(const void *const data, const unsigned int length)
{
	if (!this->allocate(length) || !memcpy(this->data, data, length))
		return false;
	this->sz = length;
	return true;
}

bool Data::load(const char *const file)
{
	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		assert(!"Rainbow::Data: Failed to load file");
		return false;
	}

	// Get size of file
	fseek(fp, 0, SEEK_END);
	const unsigned int size = ftell(fp);
	if (!this->allocate(size))
	{
		fclose(fp);
		return false;
	}

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	const unsigned int read = fread(this->data, 1, size, fp);
	fclose(fp);
	if (read != size)
	{
		delete[] this->data;
		assert(!"Rainbow::Data: Failed to read file");
		this->data = nullptr;
		this->allocated = 0;
		this->sz = 0;
		return false;
	}
	this->sz = size;

	return true;
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	assert(this->sz > 0 || !"Rainbow::Data: Data is set but size is 0");

	FILE *fp = fopen(file, "wb");
	if (!fp)
	{
		assert(!"Rainbow::Data: Failed to open file");
		return false;
	}

	// Write buffer to file
	const unsigned int written = fwrite(this->data, sizeof(unsigned char), this->sz, fp);
	fclose(fp);
	if (written != this->sz)
	{
		assert(!"Rainbow::Data: Failed to save file");
		return false;
	}

	return true;
}

#else
#	error "Unknown platform"
#endif
