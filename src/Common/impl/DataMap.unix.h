// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
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
        DataMapUnix(const uint8_t (&bytes)[N])
            : len_(N), off_(0),
              addr_(const_cast<void*>(static_cast<const void*>(bytes))),
              is_embedded_(true) {}

        explicit DataMapUnix(const filesystem::Path& path);
        DataMapUnix(DataMapUnix&& data) noexcept;
        ~DataMapUnix();

        auto data() const -> const uint8_t*
        {
            return static_cast<uint8_t*>(addr_) + off_;
        }

        void offset(size_t offset) { off_ = offset; }
        auto size() const -> size_t { return len_ - off_; }

        explicit operator bool() const { return addr_; }

    private:
        size_t len_;
        size_t off_;
        void* addr_;
        const bool is_embedded_;
    };

    using DataMap = TDataMap<DataMapUnix>;
}

#endif
