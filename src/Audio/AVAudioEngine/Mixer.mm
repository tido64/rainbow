// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/AVAudioEngine/Mixer.h"

#include <AVFoundation/AVAudioEngine.h>
#include <AVFoundation/AVAudioFile.h>
#include <AVFoundation/AVAudioMixerNode.h>
#include <AVFoundation/AVAudioPlayerNode.h>

#include "Platform/Macros.h"
#ifdef RAINBOW_OS_IOS
#    import "Audio/AVAudioEngine/RainbowAudioSession.h"
#endif

#include "Common/Logging.h"
#include "FileSystem/FileSystem.h"

using rainbow::czstring;
using rainbow::audio::AudioEngineMixer;
using rainbow::audio::Channel;
using rainbow::audio::Sound;

@interface RainbowAudioPlayerNode : AVAudioPlayerNode
@property(nonatomic, readonly, getter=isStopped) BOOL stopped;
@property(nonatomic, assign) int repeatCount;
@end

namespace
{
    AudioEngineMixer* g_instance = nullptr;

    template <typename T, typename U>
    auto bridge_cast(U _Nonnull obj) -> T*
    {
        return (__bridge T*)obj;
    }

    auto attach_audio_player_nodes(AVAudioEngine* _Nonnull engine,
                                   int num_nodes)
    {
        NSMutableArray* nodes = [NSMutableArray arrayWithCapacity:num_nodes];
        AVAudioMixerNode* main_mixer_node = engine.mainMixerNode;
        for (int i = 0; i < num_nodes; ++i)
        {
            RainbowAudioPlayerNode* node =
                [[RainbowAudioPlayerNode alloc] init];
            [engine attachNode:node];
            [engine connect:node to:main_mixer_node format:nil];
            [nodes addObject:node];
        }
        return [nodes copy];
    }
}  // namespace

bool AudioEngineMixer::initialize(int max_channels)
{
    R_ASSERT(g_instance == nullptr, "AVAudioEngine is already initialised");

    AVAudioEngine* engine = [[AVAudioEngine alloc] init];
    nodes_ = attach_audio_player_nodes(engine, max_channels);
    engine_ = engine;

#ifdef RAINBOW_OS_IOS
    audio_session_ = [RainbowAudioSession audioSessionWithMixer:this];
#endif

    g_instance = this;
    return engine != nil;
}

void AudioEngineMixer::clear()
{
    [engine_ reset];
}

void AudioEngineMixer::process() {}

void AudioEngineMixer::suspend(bool should_suspend)
{
    if (should_suspend)
    {
        [engine_ pause];
        return;
    }

    NSError* error;
    if (![engine_ startAndReturnError:&error])
    {
        LOGE("AudioEngineMixer: Failed to start engine: %s",
             error.localizedDescription.UTF8String);
    }
}

auto AudioEngineMixer::get_free_node() -> RainbowAudioPlayerNode*
{
    const NSUInteger idx =
        [nodes_ indexOfObjectPassingTest:^(
                    RainbowAudioPlayerNode* node, NSUInteger, BOOL*) {
          return node.isStopped;
        }];
    return idx == NSNotFound ? nullptr : nodes_[idx];
}

AudioEngineMixer::~AudioEngineMixer()
{
    [engine_ stop];
    g_instance = nullptr;
}

auto rainbow::audio::load_sound(czstring file) -> Sound*
{
    const auto path = filesystem::real_path(file);
    NSURL* url = [NSURL
        fileURLWithPath:[[NSString alloc]
                            initWithBytesNoCopy:const_cast<char*>(path.c_str())
                                         length:path.size()
                                       encoding:NSUTF8StringEncoding
                                   freeWhenDone:NO]
            isDirectory:NO];
    return (__bridge_retained Sound*)[[AVAudioFile alloc] initForReading:url
                                                                   error:nil];
}

auto rainbow::audio::load_stream(czstring file) -> Sound*
{
    return load_sound(file);
}

void rainbow::audio::release(Sound* sound)
{
    AVAudioFile* file = (__bridge_transfer AVAudioFile*)sound;
    file.framePosition = 0;
}

bool rainbow::audio::is_paused(Channel* channel)
{
    RainbowAudioPlayerNode* node = bridge_cast<RainbowAudioPlayerNode>(channel);
    return !node.isPlaying && !node.isStopped;
}

bool rainbow::audio::is_playing(Channel* channel)
{
    return bridge_cast<RainbowAudioPlayerNode>(channel).isPlaying;
}

void rainbow::audio::set_loop_count(Channel* channel, int count)
{
    bridge_cast<RainbowAudioPlayerNode>(channel).repeatCount = count;
}

void rainbow::audio::set_volume(Channel* channel, float volume)
{
    bridge_cast<RainbowAudioPlayerNode>(channel).volume = volume;
}

void rainbow::audio::set_world_position(Channel*, Vec2f) {}

void rainbow::audio::pause(Channel* channel)
{
    [bridge_cast<RainbowAudioPlayerNode>(channel) pause];
}

auto rainbow::audio::play(Channel* channel) -> Channel*
{
    [bridge_cast<RainbowAudioPlayerNode>(channel) play];
    return channel;
}

auto rainbow::audio::play(Sound* sound, Vec2f) -> Channel*
{
    RainbowAudioPlayerNode* node = g_instance->get_free_node();
    [node scheduleFile:bridge_cast<AVAudioFile>(sound)
                        atTime:nil
        completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
             completionHandler:^(AVAudioPlayerNodeCompletionCallbackType) {
               [node reset];
             }];

    if (!node.engine.isRunning)
        g_instance->suspend(false);

    [node play];
    return bridge_cast<Channel>(node);
}

void rainbow::audio::stop(Channel* channel)
{
    [bridge_cast<RainbowAudioPlayerNode>(channel) stop];
}

@implementation RainbowAudioPlayerNode

- (instancetype)init
{
    if (self = [super init])
    {
        _stopped = YES;
    }
    return self;
}

- (void)reset
{
    [super reset];
    _stopped = YES;
    _repeatCount = 0;
}

- (void)scheduleFile:(nonnull AVAudioFile*)file
{
    __weak __typeof(file) weakFile = file;
    __weak __typeof(self) weakSelf = self;
    [self scheduleFile:file
                        atTime:nil
        completionCallbackType:AVAudioPlayerNodeCompletionDataPlayedBack
             completionHandler:^(AVAudioPlayerNodeCompletionCallbackType) {
               __typeof(weakSelf) strongSelf = weakSelf;
               if (strongSelf == nil)
                   return;

               __typeof(weakFile) strongFile = weakFile;
               if (strongSelf.repeatCount > 0)
               {
                   --strongSelf.repeatCount;
                   [strongSelf scheduleFile:strongFile];
                   return;
               }

               [strongSelf reset];
             }];
    _stopped = NO;
}

- (void)stop
{
    _repeatCount = 0;
    [super stop];
}

@end
