/// Copyright 2010-11 Bifrost Games. All rights reserved.
/// \author Tommy Nguyen

#if defined(RAINBOW_IOS)

inline unsigned char* Data::bytes() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

inline unsigned int Data::size() const
{
	return data.length;
}

inline Data::operator void*() const
{
	return data.mutableBytes;
}

inline Data::operator unsigned char*() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

inline Data::operator NSData*() const
{
	return static_cast<NSData *>(this->data);
}

inline Data::operator NSMutableData*() const
{
	return this->data;
}

#elif defined(RAINBOW_UNIX) || defined(RAINBOW_WIN)

inline unsigned char* Data::bytes() const
{
	return this->data;
}

inline unsigned int Data::size() const
{
	return this->sz;
}

inline Data::operator void*() const
{
	return this->data;
}

inline Data::operator unsigned char*() const
{
	return this->data;
}

#else
#	error "Unknown platform"
#endif
