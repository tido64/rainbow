// Copyright 2011-13 Bifrost Entertainment. All rights reserved.

#include "Platform/Macros.h"
#ifdef RAINBOW_IOS

#include "Common/Data.h"
#include "Common/IO.h"

NSString *supportDir = nil;

Data::Data(const char *const file) : data(nil)
{
	NSString *path = Rainbow::IO::open(file, Rainbow::IO::ASSET);
	if (!path)
	{
		NSLog(@"Failed to open '%s'", file);
		return;
	}

	NSError *err = nil;
	this->data = [NSMutableData dataWithContentsOfFile:path
	                                           options:NSDataReadingUncached
	                                             error:&err];
}

Data::Data(const char *const file, unsigned int) : data(nil)
{
	NSString *path = Rainbow::IO::open(file, Rainbow::IO::USER);
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

	NSError *err = nil;
	NSString *path = Rainbow::IO::open(file, Rainbow::IO::USER);
	BOOL success = [[NSFileManager defaultManager] createFileAtPath:path
	                                                       contents:nil
	                                                     attributes:nil];

	if (!success)
		NSLog(@"Failed to open '%@'", path);
	else if (![data writeToFile:path options:0 error:&err])
	{
		NSLog(@"Failed to write '%@': %@", path, [err localizedDescription]);
		success = NO;
	}
	return success;
}

#endif
