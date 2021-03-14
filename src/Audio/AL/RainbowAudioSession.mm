// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#import "Audio/AL/RainbowAudioSession.h"

#include <AVFoundation/AVAudioSession.h>

#import "Audio/AL/Mixer.h"

#pragma clang diagnostic ignored "-Wunused-parameter"

using rainbow::audio::ALMixer;

@implementation RainbowAudioSession {
    ALMixer* _mixer;
}

+ (instancetype)audioSessionWithMixer:(ALMixer*)mixer
{
    return [[RainbowAudioSession alloc] initWithMixer:mixer];
}

- (void)dealloc
{
    [[NSNotificationCenter defaultCenter] removeObserver:self];
}

- (BOOL)setCategory:(NSString*)category
{
    if (category.length == 0)
        category = AVAudioSessionCategorySoloAmbient;

    AVAudioSession* audioSession = [AVAudioSession sharedInstance];
    NSError* error = nil;
    BOOL success = [audioSession setCategory:category error:&error];
    if (success && category == AVAudioSessionCategoryPlayAndRecord) {
        success = [audioSession
            overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker
                              error:&error];
        if (!success)
            NSLog(@"%@", error);
    }
    return success;
}

#pragma mark - Private

- (instancetype)initWithMixer:(ALMixer*)mixer
{
    if (self = [super init]) {
        _mixer = mixer;

        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(didReceiveInterruptionNotification:)
                   name:AVAudioSessionInterruptionNotification
                 object:nil];

        [[NSNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(didReceiveRouteChangeNotification:)
                   name:AVAudioSessionRouteChangeNotification
                 object:nil];

        [self activate];
    }
    return self;
}

- (void)activate
{
    NSError* error = nil;
    if (![[AVAudioSession sharedInstance] setActive:YES error:&error])
        NSLog(@"%@", error);
}

- (void)didReceiveInterruptionNotification:(NSNotification*)notification
{
    NSNumber* interuptionType =
        notification.userInfo[AVAudioSessionInterruptionTypeKey];
    switch (interuptionType.unsignedIntegerValue) {
        case AVAudioSessionInterruptionTypeBegan:
            _mixer->suspend(true);
            break;
        case AVAudioSessionInterruptionTypeEnded:
            [self activate];
            _mixer->suspend(false);
            break;
    }
}

- (void)didReceiveRouteChangeNotification:(NSNotification*)notification
{
}

@end
