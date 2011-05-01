#include "AssetManager.h"

void AssetManager::close()
{
	delete[] this->path;
	if (this->archive == 0) return;

#if defined(RAINBOW_ZIP)

	this->archive = reinterpret_cast<zip *>(zip_close(this->archive));

#endif
}

const char* AssetManager::get_full_path(const char *filename)
{
#if defined(RAINBOW_IOS)

	if (filename == 0)
		return [[archive bundlePath] UTF8String];

	NSString *file = [NSString stringWithUTF8String:(filename)];
	return [[archive pathForResource:[file stringByDeletingPathExtension] ofType:[file pathExtension]] UTF8String];

#else

	if (filename == 0)
		return RAINBOW_DATA_PATH;

	const unsigned int length = strlen(filename) + 6;
	if (this->path == 0)
	{
		this->path = new char[length];
		this->size = length;
	}
	else if (this->size < length)
	{
		delete[] this->path;
		this->path = new char[length];
		this->size = length;
	}
	strcpy(this->path, RAINBOW_DATA_PATH);
	strcat(this->path, filename);
	return this->path;

#endif
}

unsigned int AssetManager::load(unsigned char *&buffer, const char *filename)
{
	unsigned int sz = 0;

#ifdef RAINBOW_ZIP

	assert(this->archive != 0);
	filename = this->get_full_path(filename);
	zip_file *zf = zip_fopen(this->archive, filename, 0);
	assert(zf != 0 || !"Rainbow::AssetManager: Failed to load file");

	// Get uncompressed size of file
	struct zip_stat stat;
	unsigned int read = zip_stat(this->archive, filename, 0, &stat);
	assert(read == 0 || !"Rainbow::AssetManager: Failed to retrieve uncompressed size");
	sz = stat.size;

	// Allocate buffer
	buffer = new unsigned char[sz + 1];
	assert(buffer != 0 || !"Rainbow::AssetManager: Failed to allocate buffer");

	// Fill buffer
	read = zip_fread(zf, buffer, sz);
	zip_fclose(zf);
	assert(read == sz || !"Rainbow::AssetManager: Failed to uncompress file");

#else

	filename = this->get_full_path(filename);
	FILE *fp = fopen(filename, "rb");
	assert(fp != 0 || !"Rainbow::AssetManager: Failed to load file");

	// Get size of file
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);

	// Allocate buffer
	buffer = new unsigned char[sz + 1];
	assert(buffer != 0 || !"Rainbow::AssetManager: Failed to allocate buffer");

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	unsigned int read = fread(buffer, 1, sz, fp);
	fclose(fp);
	assert(read == sz || !"Rainbow::AssetManager: Failed to read file");

#endif

	return sz;
}
