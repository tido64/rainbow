#include <Rainbow/AssetManager.h>

void AssetManager::close()
{
	if (this->archive == 0) return;

#if defined(RAINBOW_ZIP)

	this->archive = reinterpret_cast<zip *>(zip_close(this->archive));

#endif
}

/// Returns the full path to a resource or asset.
const char* AssetManager::get_full_path(const char *filename)
{
#if defined(RAINBOW_ANDROID)

	unsigned int length = 0;
	while (filename[length] != '\0')
		++length;
	char *tmp = new char[length + 8];
	strcpy(tmp, "assets/");
	strcat(tmp, filename);
	return tmp;

#elif defined(RAINBOW_IOS)

	NSString *file = [NSString stringWithUTF8String:(filename)];
	NSString *path = [archive pathForResource:[file stringByDeletingPathExtension] ofType:[file pathExtension]];
	return [path UTF8String];

#else

	return filename;

#endif
}

/// Loads a file into provided buffer.
/// \param buffer    The buffer to fill
/// \param filename  The file to load
unsigned int AssetManager::load(unsigned char *&buffer, const char *filename)
{
	unsigned int sz = 0;

#ifdef RAINBOW_ZIP

	assert(this->archive != 0);
	filename = this->get_full_path(filename);
	zip_file *zf = zip_fopen(this->archive, filename, 0);
	assert(zf != 0);

	// Get uncompressed size of file
	struct zip_stat stat;
	unsigned int read = zip_stat(this->archive, filename, 0, &stat);
	assert(read == 0);
	sz = stat.size;

	// Allocate buffer
	buffer = (unsigned char *)malloc(sz + 1);
	assert(buffer != 0);

	// Fill buffer
	read = zip_fread(zf, buffer, sz);
	zip_fclose(zf);
	assert(read == sz);

#else

	filename = this->get_full_path(filename);
	FILE *fp = fopen(filename, "rb");
	assert(fp != 0);

	// Get size of file
	fseek(fp, 0, SEEK_END);
	sz = ftell(fp);

	// Allocate buffer
	buffer = (unsigned char *)malloc(sz + 1);
	assert(buffer != 0);

	// Fill buffer
	fseek(fp, 0, SEEK_SET);
	unsigned int read = fread(buffer, 1, sz, fp);
	fclose(fp);
	assert(read == sz);

#endif

	return sz;
}
