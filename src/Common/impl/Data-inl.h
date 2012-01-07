/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#if defined(RAINBOW_IOS)

inline Data::Data() : data(nil) { }

unsigned char* Data::bytes() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

unsigned int Data::size() const
{
	return data.length;
}

Data::operator bool() const
{
	return data.mutableBytes;
}

Data::operator void*() const
{
	return data.mutableBytes;
}

Data::operator unsigned char*() const
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

unsigned char* Data::bytes() const
{
	return this->data;
}

unsigned int Data::size() const
{
	return this->sz;
}

Data::operator bool() const
{
	return this->data;
}

Data::operator void*() const
{
	return this->data;
}

Data::operator unsigned char*() const
{
	return this->data;
}

#else
#	error "Unknown platform"
#endif
