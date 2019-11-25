// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Audio/Codecs/OggVorbisAudioFile.h"

#include <cstring>
#include <utility>

#include "Common/Algorithm.h"
#include "Common/Logging.h"
#include "Common/TypeCast.h"

using rainbow::czstring;
using rainbow::File;
using rainbow::audio::OggVorbisAudioFile;

namespace
{
    constexpr char kIdOggVorbis[] = "OggS";

    ov_callbacks ov_callbacks_asset{
        [](void* buffer, size_t size, size_t count, void* stream) {
            auto file = static_cast<File*>(stream);
            return file->read(buffer, size * count);
        },
        [](void* stream, ogg_int64_t offset, int origin) -> int {
            auto file = static_cast<File*>(stream);
            return !file->seek(offset, rainbow::detail::seek_origin(origin));
        },
        [](void*) { return 0; },
        [](void* stream) -> long {
            auto file = static_cast<File*>(stream);
            return file->tell();
        }};

    void ov_log_error(int err)
    {
        czstring error = "Undocumented error.";
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
}  // namespace

bool OggVorbisAudioFile::signature_matches(
    const std::array<uint8_t, 8>& signature)
{
    constexpr size_t size = array_size(kIdOggVorbis) - 1;
    return signature.size() >= size &&
           memcmp(signature.data(), kIdOggVorbis, size) == 0;
}

OggVorbisAudioFile::OggVorbisAudioFile(File f)
    : file_(std::move(f)), vf_{}, vi_(nullptr)
{
    const int result =
        ov_open_callbacks(&file_, &vf_, nullptr, 0, ov_callbacks_asset);
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
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
    auto vf = const_cast<OggVorbis_File*>(&vf_);
    return ov_pcm_total(vf, -1) * channels() * 2;
}

auto OggVorbisAudioFile::read(void* dst, size_t size) -> size_t
{
    const auto buffer = static_cast<char*>(dst);
    size_t offset = 0;
    int bitstream = 0;

    // Read until buffer is full.
    do
    {
        const int64_t read = ov_read(  //
            &vf_,
            buffer + offset,
            narrow_cast<int>(size - offset),
            /* bigendianp */ 0,
            /* word */ 2,
            /* sgned */ 1,
            &bitstream);
        if (read <= 0)
            break;

        offset += read;
    } while (offset < size);

    std::fill(buffer + offset, buffer + size, 0);
    return offset;
}

bool OggVorbisAudioFile::seek(int64_t offset)
{
    const int result = ov_raw_seek(&vf_, offset);
    if (result != 0)
    {
        ov_log_error(result);
        return false;
    }

    return true;
}
