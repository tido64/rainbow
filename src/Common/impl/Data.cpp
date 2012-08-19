// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Platform.h"
#if defined(RAINBOW_ANDROID) || defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)

#include <cstdio>
#include <cstring>

#include "Common/Data.h"

#ifdef RAINBOW_ANDROID
#include <android/asset_manager.h>
#include "LuaMachine.h"

struct AAssetManager *Data::asset_manager = nullptr;

#else
#include "Common/Debug.h"

size_t Data::data_path_length = 0;
size_t Data::userdata_path_length = 0;
char Data::data_path[] = { 0 };
char Data::userdata_path[] = { 0 };

#endif

void Data::free(const void *const p)
{
#ifdef RAINBOW_ANDROID
	static_cast<void>(p);
#else
	delete[] static_cast<const char *const>(p);
#endif
}

const char* Data::get_path(const char *const file)
{
#ifdef RAINBOW_ANDROID
	return file;
#else
	if (!file)
		return Data::data_path;

	char *path = new char[strlen(file) + Data::data_path_length];
	strcpy(path, Data::data_path);
	strcat(path, file);
	return path;
#endif
}

const char* Data::set_datapath(const char *const path)
{
#ifdef RAINBOW_ANDROID
	asset_manager = reinterpret_cast<AAssetManager*>(const_cast<char*>(path));
	return nullptr;
#else
	const size_t length = strlen(path);
	if (length >= RAINBOW_PATH_LENGTH + 1)
		return nullptr;
	Data::data_path_length = length + 2;
	Data::data_path[length] = '/';
	Data::data_path[length + 1] = '\0';
	return static_cast<const char*>(memcpy(Data::data_path, path, length));
#endif
}

const char* Data::set_userdatapath(const char *const path)
{
#ifdef RAINBOW_ANDROID
	return path;
#else
	const size_t length = strlen(path);
	if (length >= RAINBOW_PATH_LENGTH + 1)
		return nullptr;
	Data::userdata_path_length = length + 2;
	Data::userdata_path[length] = '/';
	Data::userdata_path[length + 1] = '\0';
	return static_cast<const char*>(memcpy(Data::userdata_path, path, length));
#endif
}

Data::Data(const char *const file) : allocated(0), sz(0), data(nullptr)
{
#ifdef RAINBOW_ANDROID
	AAsset *asset = AAssetManager_open(asset_manager, file, AASSET_MODE_UNKNOWN);
	if (!asset)
		return;

	const size_t size = AAsset_getLength(asset);
	this->allocate(size);

	const int read = AAsset_read(asset, this->data, size);
	AAsset_close(asset);
	if (read < 0 || static_cast<size_t>(read) != size)
#else
	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		R_ERROR("[Rainbow] Data: Failed to open '%s'\n", file);
		return;
	}

	// Get size of file
	fseek(fp, 0, SEEK_END);
	const size_t size = ftell(fp);
	this->allocate(size);

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	const size_t read = fread(this->data, 1, size, fp);
	fclose(fp);
	if (read != size)
#endif
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
	if (!this->data)
		return false;

	R_ASSERT(this->sz > 0, "Data is set but size is 0");

	FILE *fp = fopen(file, "wb");
	if (!fp)
		return false;

	// Write buffer to file
	const size_t written = fwrite(this->data, sizeof(unsigned char), this->sz, fp);
	fclose(fp);
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
