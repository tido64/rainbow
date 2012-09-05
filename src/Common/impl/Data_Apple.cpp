// Copyright 2011-12 Bifrost Entertainment. All rights reserved.

#include "Common/Data.h"
#include "Common/Debug.h"

#ifdef RAINBOW_IOS

NSURL* get_url(const char *const file)
{
	static NSURL *app_support_dir = nil;
	if (!app_support_dir)
	{
		NSFileManager *fm = [NSFileManager defaultManager];
		NSArray *dir_urls = [fm URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask];

		if ([dir_urls count] < 1)
			return nullptr;

		// Use the first directory (if multiple are returned)
		NSURL *support_dir = [dir_urls objectAtIndex:0];
		if (!support_dir)
			return nullptr;

		// If a valid app support directory exists, add the
		// app's bundle ID to it to specify the final directory.
		NSString *bundle_id = [[NSBundle mainBundle] bundleIdentifier];
		app_support_dir = [support_dir URLByAppendingPathComponent:bundle_id];
	}

	NSString *nfile = [NSString stringWithUTF8String:(file)];
	return [app_support_dir URLByAppendingPathComponent:nfile];
}

NSMutableData* open(NSString *path)
{
	R_ASSERT(path, "No file to open");

	NSError *err = nil;
	NSMutableData *data = [NSMutableData dataWithContentsOfFile:path options:NSDataReadingUncached error:&err];
	if (err != nil)
	{
		R_ERROR("[Rainbow] Data: Failed to read file");
		return nil;
	}
	return data;
}

Data::Data(const char *const file) : data(nil)
{
	NSString *path = [NSString stringWithUTF8String:(file)];
	path = [[NSBundle mainBundle] pathForResource:[path stringByDeletingPathExtension] ofType:[path pathExtension]];
	if (!path)
		return;
	this->data = open(path);
}

Data::Data(const char *const file, unsigned int) : data(nil)
{
	NSURL *url = get_url(file);
	this->data = open([url absoluteString]);
}

Data::~Data() { }

void Data::copy(const void *const data, const size_t length)
{
	[this->data replaceBytesInRange:NSMakeRange(0, this->data.length) withBytes:data length:length];
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	R_ASSERT(false, "Not implemented yet");

	return true;
}

#endif
