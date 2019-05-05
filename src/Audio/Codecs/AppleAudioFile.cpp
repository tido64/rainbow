// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/Codecs/AppleAudioFile.h"

#include "Common/Logging.h"
#include "FileSystem/FileSystem.h"

using rainbow::czstring;
using rainbow::audio::AppleAudioFile;
using rainbow::filesystem::Path;

namespace
{
    constexpr unsigned int kBitsPerChannel = 16;

    auto make_url(const Path& path) -> CFURLRef
    {
        CFStringRef str = CFStringCreateWithBytesNoCopy(  //
            kCFAllocatorDefault,
            reinterpret_cast<const UInt8*>(path.c_str()),  // NOLINT
            path.size(),
            kCFStringEncodingUTF8,
            FALSE,
            kCFAllocatorNull);
        CFURLRef url = CFURLCreateWithFileSystemPath(
            kCFAllocatorDefault, str, kCFURLPOSIXPathStyle, FALSE);
        CFRelease(str);
        return url;
    }
}  // namespace

AppleAudioFile::AppleAudioFile(czstring file) : format_{}, ref_(nullptr)
{
    const auto path = filesystem::real_path(file);
    const auto url = make_url(path);
    if (ExtAudioFileOpenURL(url, &ref_) != noErr)
        LOGE("AudioToolbox: Failed to open '%s'", file);
    CFRelease(url);
    if (ref_ == nullptr)
        return;

    UInt32 size = sizeof(format_);
    auto result = ExtAudioFileGetProperty(
        ref_, kExtAudioFileProperty_FileDataFormat, &size, &format_);
    if (result != noErr)
        LOGE("AudioToolbox: Failed to retrieve audio format.");

    FillOutASBDForLPCM(  //
        format_,
        format_.mSampleRate,
        format_.mChannelsPerFrame,
        kBitsPerChannel,
        kBitsPerChannel,
        false,
        false);

    result = ExtAudioFileSetProperty(  //
        ref_,
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
