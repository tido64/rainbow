/// Copyright 2011-12 Bifrost Entertainment. All rights reserved.
/// \author Tommy Nguyen

#if defined(RAINBOW_IOS)

Data::Data() : data(nil) { }

unsigned char* Data::bytes() const
{
	return static_cast<unsigned char *>(data.mutableBytes);
}

size_t Data::size() const
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

Data::operator char*() const
{
	return static_cast<char*>(data.mutableBytes);
}

Data::operator unsigned char*() const
{
	return static_cast<unsigned char*>(data.mutableBytes);
}

Data::operator NSData*() const
{
	return static_cast<NSData *>(this->data);
}

Data::operator NSMutableData*() const
{
	return this->data;
}

#else

Data::Data() : allocated(0), sz(0), data(nullptr) { }

unsigned char* Data::bytes() const
{
	return this->data;
}

size_t Data::size() const
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

Data::operator char*() const
{
	return reinterpret_cast<char*>(this->data);
}

Data::operator unsigned char*() const
{
	return this->data;
}

#endif
