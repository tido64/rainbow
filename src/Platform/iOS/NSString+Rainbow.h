// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import <Foundation/Foundation.h>

@interface NSString (Rainbow)

+ (nullable instancetype)stringWithUTF8StringNoCopy:(nonnull const char*)str;
+ (nullable instancetype)stringWithUTF8StringNoCopy:(nonnull const char*)str
                                             length:(uint32_t)length;

@end
