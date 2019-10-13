// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import <Foundation/Foundation.h>

namespace rainbow::audio
{
    class AudioEngineMixer;
}

NS_ASSUME_NONNULL_BEGIN

@interface RainbowAudioSession : NSObject

+ (instancetype)audioSessionWithMixer:(rainbow::audio::AudioEngineMixer*)mixer;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;

@end

NS_ASSUME_NONNULL_END
