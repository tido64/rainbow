// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"

#if defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)

#include <cassert>
#include <cstdio>
#include <cstring>

unsigned int Data::data_path_length = 0;
unsigned int Data::userdata_path_length = 0;
char Data::data_path[] = { 0 };
char Data::userdata_path[] = { 0 };

void Data::free(const void *const p)
{
	delete[] static_cast<const char *const>(p);
}

const char* Data::get_path(const char *const file)
{
	if (!file)
		return Data::data_path;

	char *path = new char[strlen(file) + Data::data_path_length];
	strcpy(path, Data::data_path);
	strcat(path, file);
	return path;
}

const char* Data::set_datapath(const char *const path)
{
	const unsigned int length = strlen(path);
	if (length >= RAINBOW_PATH_LENGTH + 1)
		return nullptr;
	Data::data_path_length = length + 2;
	Data::data_path[length] = '/';
	Data::data_path[length + 1] = '\0';
	return static_cast<const char*>(memcpy(Data::data_path, path, length));
}

const char* Data::set_userdatapath(const char *const path)
{
	const unsigned int length = strlen(path);
	if (length >= RAINBOW_PATH_LENGTH + 1)
		return nullptr;
	Data::userdata_path_length = length + 2;
	Data::userdata_path[length] = '/';
	Data::userdata_path[length + 1] = '\0';
	return static_cast<const char*>(memcpy(Data::userdata_path, path, length));
}

Data::Data(const char *const file) : allocated(0), sz(0), data(nullptr)
{
	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		fprintf(stderr, "Rainbow::Data: Failed to open '%s'\n", file);
		return;
	}

	// Get size of file
	fseek(fp, 0, SEEK_END);
	const unsigned int size = ftell(fp);
	this->allocate(size);

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	const unsigned int read = fread(this->data, 1, size, fp);
	fclose(fp);
	if (read != size)
	{
		delete[] this->data;
		this->data = nullptr;
		this->allocated = 0;
		this->sz = 0;
	}
	else
		this->sz = size;
}

Data::~Data()
{
	delete[] this->data;
}

void Data::allocate(const unsigned int size)
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

void Data::copy(const void *const data, const unsigned int length)
{
	this->allocate(length);
	memcpy(this->data, data, length);
	this->sz = length;
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	assert(this->sz > 0 || !"Rainbow::Data::save: Data is set but size is 0");

	FILE *fp = fopen(file, "wb");
	if (!fp)
		return false;

	// Write buffer to file
	const unsigned int written = fwrite(this->data, sizeof(unsigned char), this->sz, fp);
	fclose(fp);
	if (written != this->sz)
		return false;

	return true;
}

void Data::set(unsigned char *data, const unsigned int length)
{
	delete[] this->data;
	this->data = data;
	this->allocated = length;
	this->sz = length;
}

#endif
