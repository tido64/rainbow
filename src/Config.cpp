// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include "ThirdParty/DisableWarnings.h"
#include <absl/hash/hash.h>
#include "ThirdParty/ReenableWarnings.h"

#include <panini/panini.hpp>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

using namespace std::literals::string_view_literals;

namespace
{
    constexpr char kConfigINI[] = "config.ini";
    constexpr int kMaxMSAA = 16;

    struct Keys
    {
        uint64_t resolution_width;
        uint64_t resolution_height;
        uint64_t msaa;
        uint64_t allow_hidpi;
        uint64_t suspend_on_focus_lost;
        uint64_t accelerometer;
    };

    template <typename F>
    constexpr void with_bool(std::string_view value, F&& f)
    {
        if (value.empty())
            return;

        f(value != "0"sv && value != "false"sv);
    }
}  // namespace

rainbow::Config::Config()
    : width_(0), height_(0), msaa_(0), hidpi_(false), suspend_(true),
      accelerometer_(false)
{
    if (!filesystem::exists(kConfigINI))
    {
        LOGI("No config file was found");
        return;
    }

    const Data& config = File::read(kConfigINI, FileType::Asset);
    if (!config)
        return;

    absl::Hash<std::string_view> hash;
    const Keys keys{
        hash("ResolutionWidth"sv),
        hash("ResolutionHeight"sv),
        hash("MSAA"sv),
        hash("AllowHiDPI"sv),
        hash("SuspendOnFocusLost"sv),
        hash("Accelerometer"sv),
    };

    panini::parse(  //
        config.as<const char*>(),
        [this, &hash, &keys](panini::State state,
                             std::string_view section,
                             std::string_view key,
                             std::string_view value) {
            if (state == panini::State::Error)
            {
                LOGE(  //
                    "Error parsing %s:%s: %s",
                    kConfigINI,
                    section.data(),
                    key.data());
                return;
            }

            if (section != "core"sv)
                return;

            const auto hashed_key = hash(key);
            if (hashed_key == keys.resolution_width)
                width_ = atoi(value.data());
            else if (hashed_key == keys.resolution_height)
                height_ = atoi(value.data());
            else if (hashed_key == keys.msaa)
                msaa_ = floor_pow2(std::clamp(atoi(value.data()), 0, kMaxMSAA));
            else if (hashed_key == keys.allow_hidpi)
                with_bool(value, [this](bool v) { hidpi_ = v; });
            else if (hashed_key == keys.suspend_on_focus_lost)
                with_bool(value, [this](bool v) { suspend_ = v; });
            else if (hashed_key == keys.accelerometer)
                with_bool(value, [this](bool v) { accelerometer_ = v; });
        });
}
