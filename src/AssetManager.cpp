#include "AssetManager.h"

void AssetManager::close()
{
	delete[] this->path;

#if defined(RAINBOW_ZIP)

	if (this->archive == 0)
		return;
	this->archive = reinterpret_cast<zip *>(zip_close(this->archive));

#endif
}

const char* AssetManager::get_full_path(const char *const filename)
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

unsigned int AssetManager::load(void *&buffer, const char *filename)
{
#if defined(RAINBOW_IOS)

	NSString *file = [NSString stringWithUTF8String:(filename)];
	NSString *path = [[NSBundle mainBundle] pathForResource:[file stringByDeletingPathExtension] ofType:[file pathExtension]];
	NSData *texture = [[NSData alloc] initWithContentsOfFile:path];
	buffer = texture;
	return [texture length];

#else

	FILE *fp = fopen(this->get_full_path(filename), "rb");
	assert(fp != 0 || !"Rainbow::AssetManager: Failed to load file");

	// Get size of file
	fseek(fp, 0, SEEK_END);
	const unsigned int sz = ftell(fp);

	// Allocate buffer
	buffer = new unsigned char[sz + 1];
	assert(buffer != 0 || !"Rainbow::AssetManager: Failed to allocate buffer");

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	const unsigned int read = fread(buffer, 1, sz, fp);
	fclose(fp);
	assert(read == sz || !"Rainbow::AssetManager: Failed to read file");

	return (read != sz) ? 0 : sz;

#endif
}
