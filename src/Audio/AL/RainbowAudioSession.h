// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

namespace rainbow::audio
{
    class ALMixer;
}

@interface RainbowAudioSession : NSObject
+ (instancetype)audioSessionWithMixer:(rainbow::audio::ALMixer*)mixer;

- (instancetype)init UNAVAILABLE_ATTRIBUTE;
- (BOOL)setCategory:(NSString*)category;
@end
