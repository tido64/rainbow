#ifndef COMMON_IMPL_IO_ANDROID_INC_
#define COMMON_IMPL_IO_ANDROID_INC_

struct FileHandle
{
	Type type;
	union
	{
		AAsset *asset;
		FILE *file;
	};

	FileHandle(AAsset *asset) : type(kIOTypeAsset), asset(asset) { }

	FileHandle& operator=(AAsset *asset)
	{
		this->type = kIOTypeAsset;
		this->asset = asset;
		return *this;
	}

	FileHandle& operator=(FILE *file)
	{
		this->type = kIOTypeDocument;
		this->file = file;
		return *this;
	}

	operator bool() const
	{
		return this->file;
	}

	operator AAsset*() const
	{
		return this->asset;
	}

	operator FILE*() const
	{
		return this->file;
	}
};

#endif
