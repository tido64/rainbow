// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "FileSystem/Bundle.inc"

Bundle::Bundle(ArrayView<zstring>)
    : assets_path_(NSBundle.mainBundle.resourceURL.path.UTF8String),
      exec_path_(NSBundle.mainBundle.executableURL.path.UTF8String),
      main_script_(nullptr)
{
}
