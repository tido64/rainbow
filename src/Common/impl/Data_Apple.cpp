// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#ifdef RAINBOW_IOS

#include "Common/Data.h"
#include "Common/Debug.h"
#include "Common/IO.h"

namespace
{
	const char kErrorOpenFile[] = "[Rainbow] Failed to open '%s'";
}

Data::Data(const char *const file, const Type type) : data(nil)
{
	char buf[256];
	Rainbow::IO::find(buf, file, static_cast<Rainbow::IO::Type>(type));
	if (buf[0] == '\0')
	{
		R_ERROR(kErrorOpenFile, file);
		return;
	}
	NSString *path = [[NSString alloc]
			initWithBytesNoCopy:buf
			             length:strlen(buf)
			           encoding:NSUTF8StringEncoding
			       freeWhenDone:NO];
	NSError *err = nil;
	this->data = [NSMutableData dataWithContentsOfFile:path
	                                           options:NSDataReadingUncached
	                                             error:&err];
}

Data::~Data() { }

void Data::copy(const void *const data, const size_t length)
{
	if (!this->data)
		this->data = [NSMutableData dataWithLength:length];
	[this->data replaceBytesInRange:NSMakeRange(0, this->data.length)
	                      withBytes:data
	                         length:length];
}

bool Data::save(const char *const file) const
{
	if (!this->data)
		return false;

	char buf[256];
	Rainbow::IO::find(buf, file, Rainbow::IO::kIOTypeDocument);
	NSString *path = [[NSString alloc]
			initWithBytesNoCopy:buf
			             length:strlen(buf)
			           encoding:NSUTF8StringEncoding
			       freeWhenDone:NO];
	if (buf[0] == '\0')
	{
		R_ERROR(kErrorOpenFile, file);
		return false;
	}
	BOOL success = [[NSFileManager defaultManager] createFileAtPath:path
	                                                       contents:nil
	                                                     attributes:nil];
	NSError *err = nil;
	if (!success)
		R_ERROR(kErrorOpenFile, [path UTF8String]);
	else if (![data writeToFile:path options:0 error:&err])
	{
		R_ERROR("[Rainbow] Failed to write '%s': %s",
		        [path UTF8String], [[err localizedDescription] UTF8String]);
		success = NO;
	}
	return success;
}

#endif
