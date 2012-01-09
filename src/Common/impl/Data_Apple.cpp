// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"

#ifdef RAINBOW_IOS

void Data::free(const void *const p) { /* This is not needed */ }

const char* Data::get_path(const char *const file)
{
	if (!file)
		return [[[NSBundle mainBundle] bundlePath] UTF8String];

	NSString *path = [NSString stringWithUTF8String:(file)];
	return [[[NSBundle mainBundle] pathForResource:[path stringByDeletingPathExtension] ofType:[path pathExtension]] UTF8String];
}

Data::Data(const char *const file) : data(nil)
{
	this->load(file);
}

Data::~Data()
{
	[data release];
}

bool Data::copy(const void *const data, const unsigned int length)
{
	assert(!"Rainbow::Data::copy: Not implemented yet");
	return true;
}

bool Data::load(const char *const file)
{
	NSError *err = nil;
	NSString *path = [NSString stringWithUTF8String:file];
	this->data = [[NSMutableData alloc] initWithContentsOfFile:path options:NSDataReadingUncached error:&err];
	if (err != nil)
	{
		[data release];
		NSLog(@"Rainbow::Data::load: Failed to read file");
		this->data = nil;
		return false;
	}
	return true;
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	assert(!"Rainbow::Data::save: Not implemented yet");

	return true;
}

#endif
