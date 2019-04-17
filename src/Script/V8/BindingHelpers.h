// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_V8_BINDINGHELPERS_H_
#define SCRIPT_V8_BINDINGHELPERS_H_

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <v8.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

#include "Audio/Mixer.h"
#include "Common/String.h"
#include "Common/TypeCast.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/RenderQueue.h"
#include "Graphics/Sprite.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/TextureAtlas.h"
#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/VirtualKey.h"
#include "Memory/Array.h"

namespace rainbow::js
{
    struct EnumValue
    {
        czstring name;
        int integralValue;
    };

    struct MethodInfo
    {
        czstring name;
        v8::FunctionCallback callback;
        int length;
    };

    auto internalized_string(v8::Isolate* isolate, czstring str)
    {
        return v8::String::NewFromUtf8(
                   isolate, str, v8::NewStringType::kInternalized)
            .ToLocalChecked();
    }

    auto define_class(v8::Local<v8::Context> context,
                      czstring name,
                      const MethodInfo& ctor,
                      int internal_field_count,
                      ArraySpan<const MethodInfo> methods)
    {
        auto isolate = context->GetIsolate();

        auto func_template = v8::FunctionTemplate::New(isolate);
        func_template->SetClassName(internalized_string(isolate, name));
        if (ctor.callback != nullptr)
        {
            func_template->SetCallHandler(ctor.callback);
            func_template->SetLength(ctor.length);
        }

        auto instance = func_template->InstanceTemplate();
        instance->SetInternalFieldCount(internal_field_count);
        instance->SetImmutableProto();

        auto prototype = func_template->PrototypeTemplate();
        auto signature = v8::Signature::New(isolate, func_template);
        for (auto info : methods)
        {
            auto method = v8::FunctionTemplate::New(  //
                isolate,
                info.callback,
                v8::Local<v8::Value>(),
                signature,
                info.length);
            method->RemovePrototype();
            prototype->Set(  //
                internalized_string(isolate, info.name),
                method,
                static_cast<v8::PropertyAttribute>(v8::ReadOnly |
                                                   v8::DontDelete));
        }

        prototype->Set(
            v8::Symbol::GetToStringTag(isolate),
            internalized_string(isolate, name),
            static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontEnum));

        func_template->ReadOnlyPrototype();
        return func_template->GetFunction(context).ToLocalChecked();
    }

    auto define_enum(v8::Local<v8::Context> context,
                     ArraySpan<const EnumValue> values)
    {
        auto isolate = context->GetIsolate();
        auto obj =
            v8::Object::New(isolate, v8::Null(isolate), nullptr, nullptr, 0);
        for (auto value : values)
        {
            [[maybe_unused]] auto result = obj->DefineOwnProperty(
                context,
                internalized_string(isolate, value.name),
                v8::Integer::New(isolate, value.integralValue),
                static_cast<v8::PropertyAttribute>(v8::ReadOnly |
                                                   v8::DontDelete));
        }
        return obj;
    }

    auto is_construct_call(const v8::FunctionCallbackInfo<v8::Value>& info,
                           czstring message)
    {
        if (!info.IsConstructCall())
        {
            auto isolate = info.GetIsolate();
            isolate->ThrowException(
                v8::String::NewFromUtf8(
                    isolate, message, v8::NewStringType::kNormal)
                    .ToLocalChecked());
            return false;
        }

        return true;
    }

    template <typename T>
    auto get_reference(v8::Local<v8::Object> obj)
    {
        auto field = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
        return static_cast<T*>(field->Value());
    }

    template <>
    auto get_reference<SpriteRef>(v8::Local<v8::Object> obj)
    {
        auto batch = v8::Local<v8::External>::Cast(obj->GetInternalField(0));
        auto index = v8::Local<v8::Uint32>::Cast(obj->GetInternalField(1));
        return SpriteRef{
            *static_cast<SpriteBatch*>(batch->Value()), index->Value()};
    }

    template <typename T, typename... Args>
    auto make(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...).release();
    }

    template <>
    auto make<TextureAtlas, czstring&&>(czstring&& path)
    {
        auto texture = std::make_unique<TextureAtlas>(path);
        texture->retain();
        return texture.release();
    }

    void set_module(v8::Local<v8::Context> context,
                    v8::Local<v8::Object> target,
                    czstring name,
                    v8::Local<v8::Object> module)
    {
        module->SetIntegrityLevel(context, v8::IntegrityLevel::kFrozen);
        [[maybe_unused]] auto result = target->DefineOwnProperty(
            context,
            internalized_string(context->GetIsolate(), name),
            module,
            static_cast<v8::PropertyAttribute>(v8::ReadOnly | v8::DontDelete));
    }

    void set_reference(v8::Local<v8::Object> obj, void* ptr)
    {
        auto isolate = obj->GetIsolate();
        obj->SetInternalField(0, v8::External::New(isolate, ptr));
    }

    void set_reference(v8::Local<v8::Object> obj, void* ptr, uint32_t index)
    {
        auto isolate = obj->GetIsolate();
        obj->SetInternalField(0, v8::External::New(isolate, ptr));
        obj->SetInternalField(1, v8::Uint32::NewFromUnsigned(isolate, index));
    }

    template <typename T>
    auto unwrap(v8::Local<v8::Context>, v8::Local<v8::Value> value)
    {
        return v8::Local<T>::Cast(value)->Value();
    }

    template <>
    auto unwrap<bool>(v8::Local<v8::Context> context,
                      v8::Local<v8::Value> value)
    {
        return unwrap<v8::Boolean>(context, value);
    }

    template <>
    auto unwrap<float>(v8::Local<v8::Context> context,
                       v8::Local<v8::Value> value)
    {
        return static_cast<float>(unwrap<v8::Number>(context, value));
    }

    template <>
    auto unwrap<int>(v8::Local<v8::Context> context, v8::Local<v8::Value> value)
    {
        return unwrap<v8::Int32>(context, value);
    }

    template <>
    auto unwrap<uint32_t>(v8::Local<v8::Context> context,
                          v8::Local<v8::Value> value)
    {
        return unwrap<v8::Uint32>(context, value);
    }

    template <>
    auto unwrap<czstring>(v8::Local<v8::Context> context,
                          v8::Local<v8::Value> value)
    {
        if (!value->IsString())
            return "";

        static std::string buffer;

        auto str = v8::Local<v8::String>::Cast(value);
        auto isolate = context->GetIsolate();
        buffer.resize(str->Utf8Length(isolate));
        str->WriteUtf8(  //
            isolate,
            buffer.data(),
            -1,
            nullptr,
            v8::String::REPLACE_INVALID_UTF8);
        return buffer.c_str();
    }

    template <>
    auto unwrap<Animation::Callback>(v8::Local<v8::Context>,
                                     v8::Local<v8::Value> value)
    {
        if (!value->IsFunction())
            return Animation::Callback{};

        auto func = v8::Local<v8::Function>::Cast(value);
        return Animation::Callback{[func](Animation*, AnimationEvent event) {
            auto isolate = func->GetIsolate();
            auto context = isolate->GetCurrentContext();
            auto self = v8::Object::New(isolate);  // TODO
            v8::Local<v8::Value> args[]{
                self,
                v8::Uint32::NewFromUnsigned(isolate, to_underlying_type(event)),
            };
            [[maybe_unused]] auto result =
                func->CallAsFunction(context, self, array_size(args), args);
        }};
    }

    template <typename T, typename U>
    auto unwrap_number(v8::MaybeLocal<v8::Value> maybe_value, U default_value)
        -> U
    {
        v8::Local<v8::Value> value;
        return !maybe_value.ToLocal(&value) || !value->IsNumber()
                   ? default_value
                   : v8::Local<T>::Cast(value)->Value();
    }

    template <typename T>
    auto get_number(v8::Local<v8::Context> context,
                    v8::Local<v8::Value> value,
                    czstring key,
                    T default_value)
    {
        if (!value->IsObject())
            return default_value;

        auto obj = v8::Local<v8::Object>::Cast(value);
        return unwrap_number<v8::Number>(
            obj->Get(context, internalized_string(context->GetIsolate(), key)),
            default_value);
    }

    template <>
    auto unwrap<Animation::Frames>(v8::Local<v8::Context> context,
                                   v8::Local<v8::Value> value)
    {
        if (!value->IsArray())
        {
            auto empty = std::make_unique<Animation::Frame[]>(1);
            empty[0] = Animation::Frame::end();
            return empty.release();
        }

        const auto frames = v8::Local<v8::Array>::Cast(value);
        const auto count = frames->Length();
        auto copy = std::make_unique<Animation::Frame[]>(count + 1);
        for (auto i = 0u; i < count; ++i)
            copy[i] = unwrap_number<v8::Uint32>(frames->Get(context, i), 0u);
        copy[count] = Animation::Frame::end();
        return copy.release();
    }

    template <>
    auto unwrap<Color>(v8::Local<v8::Context> context,
                       v8::Local<v8::Value> value)
    {
        auto r = narrow_cast<uint8_t>(get_number(context, value, "r", 0u));
        auto g = narrow_cast<uint8_t>(get_number(context, value, "g", 0u));
        auto b = narrow_cast<uint8_t>(get_number(context, value, "b", 0u));
        auto a = narrow_cast<uint8_t>(get_number(context, value, "a", 255u));
        return Color{r, g, b, a};
    }

    template <>
    auto unwrap<SharedPtr<TextureAtlas>>(v8::Local<v8::Context>,
                                         v8::Local<v8::Value> value)
    {
        if (!value->IsObject())
            return SharedPtr<TextureAtlas>{};

        auto obj = v8::Local<v8::Object>::Cast(value);
        auto texture = get_reference<TextureAtlas>(obj);
        return SharedPtr<TextureAtlas>{texture};
    }

    template <>
    auto unwrap<SpriteRef>(v8::Local<v8::Context>, v8::Local<v8::Value> value)
    {
        if (!value->IsObject())
            return SpriteRef{};

        auto obj = v8::Local<v8::Object>::Cast(value);
        return get_reference<SpriteRef>(obj);
    }

    template <>
    auto unwrap<TextAlignment>(v8::Local<v8::Context> context,
                               v8::Local<v8::Value> value)
    {
        return static_cast<TextAlignment>(unwrap<uint32_t>(context, value));
    }

    template <>
    auto unwrap<Vec2f>(v8::Local<v8::Context> context,
                       v8::Local<v8::Value> value)
    {
        auto x = get_number(context, value, "x", 0.0f);
        auto y = get_number(context, value, "y", 0.0f);
        return Vec2f{x, y};
    }

    auto wrap(v8::Isolate* isolate, Color c)
    {
        v8::Local<v8::Name> names[]{
            internalized_string(isolate, "r"),
            internalized_string(isolate, "g"),
            internalized_string(isolate, "b"),
            internalized_string(isolate, "a"),
        };
        v8::Local<v8::Value> values[]{
            v8::Uint32::New(isolate, c.r),
            v8::Uint32::New(isolate, c.g),
            v8::Uint32::New(isolate, c.b),
            v8::Uint32::New(isolate, c.a),
        };
        return v8::Object::New(isolate, v8::Null(isolate), names, values, 4);
    }

    auto wrap(v8::Isolate* isolate, SpriteRef)
    {
        // TODO
        return v8::Object::New(isolate, v8::Null(isolate), nullptr, nullptr, 0);
    }

    auto wrap(v8::Isolate* isolate, Vec2f v)
    {
        v8::Local<v8::Name> names[]{
            internalized_string(isolate, "x"),
            internalized_string(isolate, "y"),
        };
        v8::Local<v8::Value> values[]{
            v8::Number::New(isolate, v.x),
            v8::Number::New(isolate, v.y),
        };
        return v8::Object::New(isolate, v8::Null(isolate), names, values, 2);
    }
}  // namespace rainbow::js

#endif
