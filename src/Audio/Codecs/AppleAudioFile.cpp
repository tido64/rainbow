// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/Codecs/AppleAudioFile.h"

#include "Common/Logging.h"
#include "FileSystem/FileSystem.h"

using rainbow::czstring;
using rainbow::audio::AppleAudioFile;

namespace
{
    constexpr unsigned int kBitsPerChannel = 16;

    template <typename C, typename T>
    auto bridge_cast(T& var) -> C
    {
#ifdef RAINBOW_OS_IOS
        return (__bridge C)var;
#else
        return var;
#endif
    }
}  // namespace

AppleAudioFile::AppleAudioFile(czstring file) : format_{}, ref_(nullptr)
{
    const auto path = filesystem::relative(file);
#ifdef RAINBOW_OS_MACOS
    CFURLRef url = filesystem::make_cfurl(path);
#else
    NSURL* url = filesystem::make_nsurl(path);
#endif
    if (ExtAudioFileOpenURL(bridge_cast<CFURLRef>(url), &ref_) != noErr)
        LOGE("AudioToolbox: Failed to open '%s'", file);
#ifdef RAINBOW_OS_MACOS
    CFRelease(url);
#endif
    if (ref_ == nullptr)
        return;

    UInt32 size = sizeof(format_);
    auto result = ExtAudioFileGetProperty(
        ref_, kExtAudioFileProperty_FileDataFormat, &size, &format_);
    if (result != noErr)
        LOGE("AudioToolbox: Failed to retrieve audio format.");

    FillOutASBDForLPCM(format_,
                       format_.mSampleRate,
                       format_.mChannelsPerFrame,
                       kBitsPerChannel,
                       kBitsPerChannel,
                       false,
                       false);

    result = ExtAudioFileSetProperty(ref_,
                                     kExtAudioFileProperty_ClientDataFormat,
                                     sizeof(format_),
                                     &format_);
    if (result != noErr)
        LOGE("AudioToolbox: Failed to set client data format.");
}

AppleAudioFile::~AppleAudioFile()
{
    if (ref_ != nullptr)
        ExtAudioFileDispose(ref_);
}

auto AppleAudioFile::size() const -> size_t
{
    SInt64 frames = 0;
    UInt32 size = sizeof(frames);
    const auto result = ExtAudioFileGetProperty(
        ref_, kExtAudioFileProperty_FileLengthFrames, &size, &frames);
    if (result != noErr)
    {
        LOGE("AudioToolbox: Failed to retrieve audio length.");
        return 0;
    }

    return frames * format_.mBytesPerFrame;
}

auto AppleAudioFile::read(void* dst, size_t size) -> size_t
{
    UInt32 frames = size / format_.mBytesPerFrame;
    AudioBufferList buffer{};
    buffer.mNumberBuffers = 1;
    buffer.mBuffers[0].mNumberChannels = format_.mChannelsPerFrame;
    buffer.mBuffers[0].mDataByteSize = size;
    buffer.mBuffers[0].mData = dst;
    if (ExtAudioFileRead(ref_, &frames, &buffer) != noErr)
        LOGE("AudioToolbox: Failed to read <%p>", static_cast<void*>(ref_));
    return frames * static_cast<size_t>(format_.mBytesPerFrame);
}

bool AppleAudioFile::seek(int64_t offset)
{
    return ExtAudioFileSeek(ref_, offset) == noErr;
}
