// Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Common/Data.h"

#include <cstring>

#include "Common/Logging.h"
#include "FileSystem/File.h"

using rainbow::Data;
using rainbow::File;

auto Data::load_asset(const char* asset) -> Data
{
    return Data(File::open_asset(asset));
}

auto Data::load_document(const char* document) -> Data
{
    return Data(File::open_document(document));
}

Data::Data(File&& file)
    : ownership_(Ownership::Owner), allocated_(0), sz_(0), data_(nullptr)
{
    if (!file)
        return;

    const size_t size = file.size();
    allocate(size);
    if (file.read(data_, size) != size)
    {
        operator delete(data_);
        data_ = nullptr;
        allocated_ = 0;
    }
    else
        sz_ = size;
}

Data::~Data()
{
    if (ownership_ != Ownership::Owner)
        return;

    operator delete(data_);
}

bool Data::save(const char* path) const
{
    R_ASSERT(data_, "No data to save");
    R_ASSERT(sz_ > 0, "Data is set but size is 0");

    File file = File::open_write(path);
    return file && file.write(data_, sz_) == sz_;
}

void Data::allocate(size_t size)
{
    R_ASSERT(ownership_ == Ownership::Owner,
             "Cannot reallocate a read-only buffer");

    if (size >= allocated_)
    {
        operator delete(data_);
        allocated_ = size + 1;
        data_ = operator new(allocated_);
        static_cast<char*>(data_)[size] = 0;
    }
    else  // Pad with zeros.
    {
        memset(static_cast<char*>(data_) + size, 0, allocated_ - size);
    }
}
