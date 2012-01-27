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
	NSError *err = nil;
	NSString *path = [NSString stringWithUTF8String:file];
	this->data = [[NSMutableData alloc] initWithContentsOfFile:path options:NSDataReadingUncached error:&err];
	if (err != nil)
	{
		[data release];
		NSLog(@"Rainbow::Data: Failed to read file");
		this->data = nil;
	}
}

Data::~Data()
{
	[data release];
}

void Data::copy(const void *const data, const unsigned int length)
{
	R_ASSERT(false, "copy: Not implemented yet");
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	R_ASSERT(false, "save: Not implemented yet");

	return true;
}

#endif
