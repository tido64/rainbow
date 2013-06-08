#ifndef IO_ANDROID_INC_
#define IO_ANDROID_INC_

namespace
{
	const char kErrorAccessDirectory[] = "Error accessing '%s' (%x)\n";
	const char kErrorCreateDirectory[] = "Failed to create directory '%s' (%x)\n";
	const char kErrorInvalidDirectory[] = "'%s' is not a valid directory\n";

	/// Make a directory, including any intermediate directories.
	/// \see mkdir()
	/// \see stat()
	/// \param path  Directory path to make.
	/// \param mode  Permission bits to set on created directories.
	/// \return 0 upon success. Otherwise, -1 is returned and errno is set to
	///         indicate the error.
	int mkdirp(const char *const path, const mode_t mode)
	{
		bool end = false;
		struct stat sb;
		char dir[256];
		strcpy(dir, path);
		for (int i = 1; !end; ++i)
		{
			end = dir[i] == '\0';
			if (dir[i] == '/' || end)
			{
				dir[i] = '\0';
				if (stat(dir, &sb) != 0)
				{
					if (errno != ENOENT)
					{
						R_ERROR(kErrorAccessDirectory, dir, errno);
						return -1;
					}
					if (mkdir(dir, mode) != 0 || stat(dir, &sb) != 0)
					{
						R_ERROR(kErrorCreateDirectory, dir, errno);
						return -1;
					}
				}
				if ((sb.st_mode & S_IFDIR) == 0)
				{
					R_ERROR(kErrorInvalidDirectory, dir);
					return -1;
				}
				dir[i] = '/';
			}
		}
		return 0;
	}
}

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
