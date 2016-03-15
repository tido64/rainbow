// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/Codecs/OggVorbisAudioFile.h"

#include <cstring>
#include <utility>

#include "Common/Algorithm.h"
#include "Common/Logging.h"

using rainbow::audio::OggVorbisAudioFile;

namespace
{
    constexpr const char kIdOggVorbis[] = "OggS";

    void ov_log_error(int err)
    {
        const char* error = "Undocumented error.";
        switch (err)
        {
            case OV_EREAD:
                error = "A read from media returned an error.";
                break;
            case OV_EFAULT:
                error =
                    "Internal logic fault; indicates a bug or heap/stack "
                    "corruption.";
                break;
            case OV_ENOTVORBIS:
                error = "Bitstream does not contain any Vorbis data.";
                break;
            case OV_EBADHEADER:
                error = "Invalid Vorbis bitstream header.";
                break;
            case OV_EVERSION:
                error = "Vorbis version mismatch.";
                break;
            case OV_EIMPL:
            case OV_EINVAL:
            case OV_ENOTAUDIO:
            case OV_EBADPACKET:
            case OV_EBADLINK:
            case OV_ENOSEEK:
            default:
                break;
        }
        LOGE("Vorbis: %s", error);
    }
}

bool OggVorbisAudioFile::signature_matches(const ArrayView<char>& id)
{
    const size_t size = array_size(kIdOggVorbis);
    return id.size() >= size && strncmp(id.begin(), kIdOggVorbis, size) == 0;
}

OggVorbisAudioFile::OggVorbisAudioFile(File&& f)
    : file_(std::move(f)), vi_(nullptr)
{
    const int result = ov_open_callbacks(
        static_cast<FILE*>(file_), &vf_, nullptr, 0, OV_CALLBACKS_DEFAULT);
    if (result < 0)
    {
        ov_log_error(result);
        return;
    }

    vi_ = ov_info(&vf_, -1);
    if (vi_ == nullptr)
    {
        ov_clear(&vf_);
        LOGE("Vorbis: Failed to retrieve Ogg bitstream info.");
    }
}

OggVorbisAudioFile::~OggVorbisAudioFile()
{
    if (vi_ != nullptr)
        ov_clear(&vf_);
}

auto OggVorbisAudioFile::size() const -> size_t
{
    return ov_pcm_total(const_cast<OggVorbis_File*>(&vf_), -1) * channels() * 2;
}

auto OggVorbisAudioFile::read(void* dst, size_t size) -> size_t
{
    long read = 0;
    int bitstream = 0;
    char* buffer = static_cast<char*>(dst);
    size_t offset = 0;

    // Read until buffer is full.
    do
    {
        read =
            ov_read(&vf_, buffer + offset, size - offset, 0, 2, 1, &bitstream);
        if (read <= 0)
            break;

        offset += read;
    } while (offset < size);

    std::fill(buffer + offset, buffer + size, 0);
    return offset;
}

auto OggVorbisAudioFile::seek(long offset, int) -> int
{
    const int result = ov_raw_seek(&vf_, offset);
    if (result != 0)
        ov_log_error(result);
    return result;
}
