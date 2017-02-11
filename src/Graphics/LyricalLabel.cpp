// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Graphics/LyricalLabel.h"

#include "Common/Random.h"
#include "Script/Timer.h"

using rainbow::Color;
using rainbow::LyricalLabel;
using rainbow::Vec2i;
using rainbow::Vec2u;
using rainbow::czstring;

namespace
{
    constexpr uint32_t kStaleAttribute = 1u << 16;
}

LyricalLabel::LyricalLabel() : applied_(0), did_shake_(false)
{
    std::uninitialized_fill_n(animators_, array_size(animators_), nullptr);
}

LyricalLabel::~LyricalLabel()
{
    for (auto&& animator : animators_)
    {
        if (animator != nullptr)
            TimerManager::Get()->clear_timer(animator);
    }
}

void LyricalLabel::clear_animations()
{
    stop_animation(Animation::Shake);
    stop_animation(Animation::Typing);
}

void LyricalLabel::clear_attributes()
{
    undo_from(attributes_.cbegin());
    attributes_.clear();
}

void LyricalLabel::clear_attributes(Attribute::Type type)
{
    switch (type)
    {
        case Attribute::Type::Shake:
            stop_animation(Animation::Shake);
            break;
        default: {
            auto begin = std::remove_if(
                attributes_.begin(),
                attributes_.end(),
                [type](const Attribute& attr) { return attr.type == type; });
            if (begin == attributes_.end())
                break;
            undo_from(begin);
            attributes_.erase(begin, attributes_.end());
            break;
        }
    }
}

void LyricalLabel::set_color(Color c, uint32_t start, uint32_t length)
{
    attributes_.emplace_back(c, start, length);
    set_needs_update(kStaleAttribute);
}

void LyricalLabel::set_text(czstring text)
{
    clear_animations();
    Label::set_text(text);
}

void LyricalLabel::set_offset(const Vec2i& offset,
                              uint32_t start,
                              uint32_t length)
{
    attributes_.emplace_back(offset, start, length);
    set_needs_update(kStaleAttribute);
}

void LyricalLabel::set_shaking(uint32_t magnitude,
                               uint32_t start,
                               uint32_t length)
{
    attributes_.emplace_back(magnitude, start, length);
    if (animators_[static_cast<int>(Animation::Shake)] == nullptr)
        start_animation(Animation::Shake, 15);
}

void LyricalLabel::start_animation(Animation animation, int interval)
{
    auto i = static_cast<int>(animation);
    if (animators_[i] != nullptr)
        return;

    switch (animation)
    {
        case Animation::Shake:
            animators_[i] = TimerManager::Get()->set_timer(
                [this] {
                    // Prevent multiple calls before an update.
                    if (did_shake_)
                        return;

                    clear_attributes(Attribute::Type::Offset);
                    const uint32_t size =
                        static_cast<uint32_t>(attributes_.size());
                    for (uint32_t i = 0; i < size; ++i)
                    {
                        auto&& attr = attributes_[i];
                        if (attr.type == Attribute::Type::Shake)
                        {
                            for (uint32_t j = 0; j < attr.length; ++j)
                            {
                                const Vec2i magnitude(random(attr.magnitude),
                                                      random(attr.magnitude));
                                set_offset(magnitude, attr.start + j, 1);
                            }
                        }
                    }
                    did_shake_ = true;
                },
                interval, -1);
            did_shake_ = false;
            break;
        case Animation::Typing:
            set_cutoff(0);
            animators_[i] = TimerManager::Get()->set_timer(
                [this] { set_cutoff(cutoff() + 1); }, interval, length() - 1);
            break;
        default:
            break;
    }
}

void LyricalLabel::stop_animation(Animation animation)
{
    const int i = static_cast<int>(animation);
    if (animators_[i] == nullptr)
        return;

    switch (animation)
    {
        case Animation::Shake:
            clear_attributes(Attribute::Type::Offset);
            remove_if(  //
                attributes_,
                [](const Attribute& attr) {
                    return attr.type == Attribute::Type::Shake;
                });
            break;
        case Animation::Typing:
            set_cutoff(std::numeric_limits<decltype(cutoff())>::max());
            break;
        default:
            R_ASSERT(false, "This shouldn't ever happen.");
            UNREACHABLE();
            return;
    }
    TimerManager::Get()->clear_timer(animators_[i]);
    animators_[i] = nullptr;
}

void LyricalLabel::update()
{
    if (state() != 0)
    {
        if ((state() & kStaleMask) != 0)
        {
            update_internal();
            applied_ = 0;
        }

        auto buffer = vertex_buffer();
        for (; applied_ < attributes_.size(); ++applied_)
        {
            const auto& attr = attributes_[applied_];
            const Vec2u interval = get_interval(attr);
            switch (attr.type)
            {
                case Attribute::Type::Color:
                    for (uint32_t i = interval.x; i < interval.y; ++i)
                    {
                        buffer[i].color.r = attr.color[0];
                        buffer[i].color.g = attr.color[1];
                        buffer[i].color.b = attr.color[2];
                        buffer[i].color.a = attr.color[3];
                    }
                    break;
                case Attribute::Type::Offset:
                    for (uint32_t i = interval.x; i < interval.y; ++i)
                    {
                        buffer[i].position.x += attr.offset[0];
                        buffer[i].position.y += attr.offset[1];
                    }
                    break;
                default:
                    break;
            }
        }

        upload();
        clear_state();
        did_shake_ = false;
    }
}

auto LyricalLabel::get_interval(const Attribute& attr) -> Vec2u
{
    const uint32_t final = static_cast<uint32_t>(length()) * 4;
    return {std::min(attr.start * 4, final),
            std::min((attr.start + attr.length) * 4, final)};
}

void LyricalLabel::undo_from(std::vector<Attribute>::const_iterator first)
{
    if (applied_ != attributes_.size())
    {
        set_needs_update(kStaleBuffer);
        return;
    }

    auto buffer = vertex_buffer();
    for (auto attr = first; attr != attributes_.cend(); ++attr)
    {
        const Vec2u interval = get_interval(*attr);
        switch (attr->type)
        {
            case Attribute::Type::Color: {
                auto c = color();
                for (uint32_t i = interval.x; i < interval.y; ++i)
                    buffer[i].color = c;
                break;
            }
            case Attribute::Type::Offset:
                for (uint32_t i = interval.x; i < interval.y; ++i)
                {
                    buffer[i].position.x -= attr->offset[0];
                    buffer[i].position.y -= attr->offset[1];
                }
                break;
            default:
                break;
        }
    }
    set_needs_update(kStaleAttribute);
    applied_ -= std::distance(first, attributes_.cend());
}

LyricalLabel::Attribute::Attribute(Color c, uint32_t start, uint32_t len)
    : type(Type::Color), start(start), length(len)
{
    color[0] = c.r;
    color[1] = c.g;
    color[2] = c.b;
    color[3] = c.a;
}

LyricalLabel::Attribute::Attribute(uint32_t magnitude,
                                   uint32_t start,
                                   uint32_t len)
    : type(Type::Shake), start(start), length(len), magnitude(magnitude)
{
}

LyricalLabel::Attribute::Attribute(const Vec2i& offset_,
                                   uint32_t start,
                                   uint32_t len)
    : type(Type::Offset), start(start), length(len)
{
    offset[0] = offset_.x;
    offset[1] = offset_.y;
}
