// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Config.h"

#include <array>

#include <rapidjson/error/en.h>
#include <rapidjson/reader.h>
#include <rapidjson/stream.h>

#include "Common/Algorithm.h"
#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "FileSystem/FileSystem.h"

namespace
{
    constexpr unsigned int kMaxMSAA = 16;
}  // namespace

class rainbow::detail::ConfigSAXParser final
    : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, ConfigSAXParser>
{
public:
    explicit ConfigSAXParser(Config& config)
        : current_key_(nullptr), config_(config)
    {
    }

    bool Default() { return false; }

    bool Bool(bool val)
    {
        switch (current_key())
        {
            case ConfigKey::Accelerometer:
                config_.accelerometer_ = val;
                break;
            case ConfigKey::AllowHighDPI:
                config_.high_dpi_ = val;
                break;
            case ConfigKey::SuspendOnFocusLost:
                config_.suspend_ = val;
                break;
            default:
                return false;
        }

        pop();
        return true;
    }

    bool Uint(unsigned int val)
    {
        switch (parent_key())
        {
            case ConfigKey::Root:
                switch (current_key())
                {
                    case ConfigKey::MSAA:
                        config_.msaa_ = std::min(floor_pow2(val), kMaxMSAA);
                        break;
                    default:
                        return false;
                }
                break;

            case ConfigKey::Resolution:
                switch (current_key())
                {
                    case ConfigKey::Width:
                        config_.width_ = val;
                        break;
                    case ConfigKey::Height:
                        config_.height_ = val;
                        break;
                    default:
                        return false;
                }
                break;

            default:
                return false;
        }

        pop();
        return true;
    }

    bool String(czstring, size_t, bool)
    {
        const auto is_schema = current_key() == ConfigKey::Schema;
        pop();
        return is_schema;
    }

    bool StartObject()
    {
        if (current_key_ == nullptr)
        {
            stack_[0] = ConfigKey::Root;
            current_key_ = stack_.data();
            return true;
        }

        return parent_key() == ConfigKey::Root &&
               current_key() == ConfigKey::Resolution;
    }

    bool Key(const char* str, size_t length, bool)
    {
        const std::string_view val(str, length);
        switch (current_key())
        {
            case ConfigKey::Root:
                if (val == "$schema")
                    push(ConfigKey::Schema);
                else if (val == "accelerometer")
                    push(ConfigKey::Accelerometer);
                else if (val == "allowHighDPI")
                    push(ConfigKey::AllowHighDPI);
                else if (val == "msaa")
                    push(ConfigKey::MSAA);
                else if (val == "resolution")
                    push(ConfigKey::Resolution);
                else if (val == "suspendOnFocusLost")
                    push(ConfigKey::SuspendOnFocusLost);
                else
                    return false;
                break;

            case ConfigKey::Resolution:
                if (val == "width")
                    push(ConfigKey::Width);
                else if (val == "height")
                    push(ConfigKey::Height);
                else
                    return false;
                break;

            default:
                return false;
        }
        return true;
    }

    bool EndObject(size_t)
    {
        pop();
        return true;
    }

private:
    enum class ConfigKey
    {
        Root,
        Accelerometer,
        AllowHighDPI,
        Height,
        MSAA,
        Resolution,
        Schema,
        SuspendOnFocusLost,
        Width,
    };

    std::array<ConfigKey, 3> stack_;
    ConfigKey* current_key_;
    Config& config_;

    auto current_key() const -> ConfigKey
    {
        R_ASSERT(current_key_ != nullptr, "Malformed JSON");
        return *current_key_;
    }

    auto parent_key() const -> ConfigKey
    {
        R_ASSERT(current_key_ > stack_.data(), "Malformed JSON");
        return *(current_key_ - 1);
    }

    void pop() { --current_key_; }

    void push(ConfigKey key) { *(++current_key_) = key; }
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
    rapidjson::StringStream is{config.as<const char*>()};
    detail::ConfigSAXParser handler{*this};
    if (!reader.Parse<rapidjson::kParseStopWhenDoneFlag>(is, handler))
    {
        const auto offset = reader.GetErrorOffset();
        std::string context(
            std::string_view(config.as<const char*>(), config.size())
                .substr(offset, 8));
        if (context.length() == 8)
            context.append(3, '.');
        LOGE("config.json +%zugo near '%s': %s",
             offset,
             context.c_str(),
             rapidjson::GetParseError_En(reader.GetParseErrorCode()));
    }
}
