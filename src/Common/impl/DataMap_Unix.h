// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef COMMON_IMPL_DATAMAP_UNIX_H_
#define COMMON_IMPL_DATAMAP_UNIX_H_

namespace rainbow
{
    class DataMapUnix
    {
    protected:
        template <size_t N>
        DataMapUnix(const byte_t (&bytes)[N])
            : len_(N), off_(0),
              addr_(const_cast<void*>(static_cast<const void*>(bytes))),
              mmapped_(false) {}

        explicit DataMapUnix(const Path& path);
        DataMapUnix(DataMapUnix&& data);
        ~DataMapUnix();

        const byte_t* data() const
        {
            return static_cast<byte_t*>(addr_) + off_;
        }

        void offset(size_t offset) { off_ = offset; }
        size_t size() const { return len_ - off_; }

        explicit operator bool() const { return addr_; }

    private:
        size_t len_;
        size_t off_;
        void* addr_;
        const bool mmapped_;
    };
}

using DataMap = rainbow::TDataMap<rainbow::DataMapUnix>;

#endif
