// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <string_view>

#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

namespace
{
    constexpr unsigned int kMaxMSAA = 16;

    enum class ConfigKey
    {
        Root,
        Accelerometer,
        AllowHighDPI,
        MSAA,
        Resolution,
        ResolutionWidth,
        ResolutionHeight,
        SuspendOnFocusLost,
    };

    template <size_t N>
    constexpr auto make_string(const char (&str)[N])
    {
        return std::string_view{str, N - 1};
    }

}  // namespace

class rainbow::ConfigReaderHandler
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>,
                                          ConfigReaderHandler>
{
public:
    ConfigReaderHandler(Config& config)
        : config_(config), key_(ConfigKey::Root), level_(0)
    {
    }

    bool Default() { return false; }

    bool Bool(bool value)
    {
        switch (key_)
        {
            case ConfigKey::Accelerometer:
                config_.accelerometer_ = value;
                break;
            case ConfigKey::AllowHighDPI:
                config_.high_dpi_ = value;
                break;
            case ConfigKey::SuspendOnFocusLost:
                config_.suspend_ = value;
                break;
            default:
                return false;
        }

        return true;
    }

    bool Int(int value)
    {
        switch (key_)
        {
            case ConfigKey::MSAA:
                config_.msaa_ = std::min(floor_pow2(value), kMaxMSAA);
                break;
            case ConfigKey::ResolutionWidth:
                config_.width_ = value;
                break;
            case ConfigKey::ResolutionHeight:
                config_.height_ = value;
                break;
            default:
                return false;
        }
        return true;
    }

    bool Uint(unsigned value) { return Int(static_cast<int>(value)); }
    bool Int64(int64_t value) { return Int(static_cast<int>(value)); }
    bool Uint64(uint64_t value) { return Int(static_cast<int>(value)); }
    bool Double(double value) { return Int(static_cast<int>(value)); }

    bool StartObject()
    {
        ++level_;
        return true;
    }

    bool Key(const char* str, size_t len, bool)
    {
        const std::string_view key{str, len};
        switch (key_)
        {
            case ConfigKey::Resolution:
            case ConfigKey::ResolutionWidth:
            case ConfigKey::ResolutionHeight:
                if (level_ != 2)
                    return false;
                if (key == make_string("width"))
                    key_ = ConfigKey::ResolutionWidth;
                else if (key == make_string("height"))
                    key_ = ConfigKey::ResolutionHeight;
                break;

            default:
                if (level_ != 1)
                    return false;
                if (key == make_string("accelerometer"))
                    key_ = ConfigKey::Accelerometer;
                else if (key == make_string("allowHighDPI"))
                    key_ = ConfigKey::AllowHighDPI;
                else if (key == make_string("msaa"))
                    key_ = ConfigKey::MSAA;
                else if (key == make_string("resolution"))
                    key_ = ConfigKey::Resolution;
                else if (key == make_string("suspendOnFocusLost"))
                    key_ = ConfigKey::SuspendOnFocusLost;
                else
                    key_ = ConfigKey::Root;
                break;
        }
        return true;
    }

    bool EndObject(size_t)
    {
        key_ = ConfigKey::Root;
        --level_;
        return true;
    }

private:
    Config& config_;
    ConfigKey key_;
    int level_;
};

rainbow::Config::Config()
    : accelerometer_(true), high_dpi_(false), suspend_(true), width_(0),
      height_(0), msaa_(0)
{
    constexpr char kConfigModule[] = "config.json";

    const auto path = filesystem::relative(kConfigModule);
    std::error_code error;
    if (!filesystem::is_regular_file(path, error))
    {
        LOGI("No config file was found");
        return;
    }

    const Data& config = File::read(kConfigModule, FileType::Asset);
    if (!config)
        return;

    rapidjson::Reader reader;
    ConfigReaderHandler handler(*this);
    rapidjson::StringStream ss(config.as<const char*>());
    if (!reader.Parse<rapidjson::kParseStopWhenDoneFlag>(ss, handler))
    {
        LOGE("config.json:%zu: %s",
             reader.GetErrorOffset(),
             rapidjson::GetParseError_En(reader.GetParseErrorCode()));
    }
}
