#include "Common/Data.h"

bool Data::load(const char *const file)
{
#if defined(RAINBOW_IOS)

	NSError *err = nil;
	NSString *str = [NSString stringWithUTF8String:(file)];
	this->data = [[NSMutableData alloc] initWithContentsOfFile:str options:NSDataReadingUncached error:&err];
	if (err != nil)
	{
		[data release];
		NSLog(@"Rainbow::Data: Failed to read file");
		this->data = nil;
		return false;
	}

#else

	FILE *fp = fopen(file, "rb");
	if (!fp)
	{
		assert(!"Rainbow::Data: Failed to load file");
		return false;
	}

	// Get size of file
	fseek(fp, 0, SEEK_END);
	const unsigned int size = ftell(fp);

	// Allocate buffer
	if (size > this->sz)  // The data needs a bigger buffer
	{
		delete[] this->data;
		this->data = new unsigned char[size];
		if (!this->data)
		{
			fclose(fp);
			assert(!"Rainbow::Data: Failed to allocate buffer");
			this->sz = 0;
			return false;
		}
	}
	else  // Pad with zeros
		memset(this->data + size, 0, this->sz - size);

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	const unsigned int read = fread(this->data, 1, size, fp);
	fclose(fp);
	if (read != size)
	{
		delete[] this->data;
		assert(!"Rainbow::Data: Failed to read file");
		this->data = 0;
		this->sz = 0;
		return false;
	}
	this->sz = size;

#endif

	return true;
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

#if defined(RAINBOW_IOS)

	return false;

#else

	FILE *fp = fopen(file, "wb");
	if (!fp)
	{
		assert(!"Rainbow::Data: Failed to open file");
		return false;
	}

	// Write buffer to file
	const unsigned int written = fwrite(this->data, 1, this->sz, fp);
	fclose(fp);
	if (written != this->sz)
	{
		assert(!"Rainbow::Data: Failed to save file");
		return false;
	}

#endif

	return true;
}
