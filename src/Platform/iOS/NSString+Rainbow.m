// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "Platform/iOS/NSString+Rainbow.h"

@implementation NSString (Rainbow)

+ (nullable instancetype)stringWithUTF8StringNoCopy:(nonnull const char*)str
{
    return [NSString stringWithUTF8StringNoCopy:str length:strlen(str)];
}

+ (nullable instancetype)stringWithUTF8StringNoCopy:(nonnull const char*)str
                                             length:(uint32_t)length
{
    return [[NSString alloc] initWithBytesNoCopy:(void*)str
                                          length:length
                                        encoding:NSUTF8StringEncoding
                                    freeWhenDone:NO];
}

@end
