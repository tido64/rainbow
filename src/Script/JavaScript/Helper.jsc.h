// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#ifndef SCRIPT_JAVASCRIPT_HELPER_JSC_H_
#define SCRIPT_JAVASCRIPT_HELPER_JSC_H_

#include <JavaScriptCore/JavaScript.h>

#include <Rainbow/AnimationEvent.h>
#include <Rainbow/TextAlignment.h>

#include "Audio/Mixer.h"
#include "Graphics/Animation.h"
#include "Graphics/Label.h"
#include "Graphics/RenderQueue.h"
#include "Graphics/Sprite.h"
#include "Graphics/SpriteBatch.h"
#include "Graphics/TextureAtlas.h"
#include "Input/Controller.h"
#include "Input/Input.h"
#include "Input/VirtualKey.h"

namespace rainbow::jsc
{
    template <typename T, typename Deleter>
    using unique_ref = std::unique_ptr<std::remove_pointer_t<T>, Deleter>;

    struct JSClassReleaser
    {
        void operator()(JSClassRef js_class) const { JSClassRelease(js_class); }
    };

    struct JSStringReleaser
    {
        void operator()(JSStringRef string) const { JSStringRelease(string); }
    };

    class UniqueClassRef
    {
    public:
        UniqueClassRef(const JSClassDefinition* definition)
            : class_(JSClassCreate(definition))
        {
        }

        operator JSClassRef() { return class_.get(); }

    private:
        unique_ref<JSClassRef, JSClassReleaser> class_;
    };

    class UniqueStringRef
    {
    public:
        UniqueStringRef(czstring string)
            : string_(JSStringCreateWithUTF8CString(string))
        {
        }

        operator JSStringRef() { return string_.get(); }

    private:
        unique_ref<JSStringRef, JSStringReleaser> string_;
    };

    auto create_class_definition(czstring class_name,
                                 const JSStaticValue* static_values,
                                 const JSStaticFunction* static_functions,
                                 JSObjectFinalizeCallback finalize,
                                 JSObjectCallAsConstructorCallback constructor)
        -> JSClassDefinition
    {
        return {
            0,                      // version
            kJSClassAttributeNone,  // attributes
            class_name,             // className
            nullptr,                // parentClass
            static_values,          // staticValues
            static_functions,       // staticFunctions
            nullptr,                // initialize
            finalize,               // finalize
            nullptr,                // hasProperty
            nullptr,                // getProperty
            nullptr,                // setProperty
            nullptr,                // deleteProperty
            nullptr,                // getPropertyNames
            nullptr,                // callAsFunction
            constructor,            // callAsConstructor
            nullptr,                // hasInstance
            nullptr,                // convertToType
        };
    }

    template <typename T>
    auto get_instance(JSObjectRef object)
    {
        return static_cast<T*>(JSObjectGetPrivate(object));
    }

    template <>
    auto get_instance<SpriteRef>(JSObjectRef object)
    {
        return *static_cast<SpriteRef*>(JSObjectGetPrivate(object));
    }

    template <>
    auto get_instance<SharedPtr<TextureAtlas>>(JSObjectRef object)
    {
        return static_cast<TextureAtlas*>(JSObjectGetPrivate(object));
    }

    template <typename T>
    auto make_value(JSContextRef ctx, T) -> JSValueRef;

    template <>
    auto make_value<std::nullptr_t>(JSContextRef ctx, std::nullptr_t)
        -> JSValueRef
    {
        return JSValueMakeUndefined(ctx);
    }

    template <>
    auto make_value<bool>(JSContextRef ctx, bool value) -> JSValueRef
    {
        return JSValueMakeBoolean(ctx, value);
    }

    template <>
    auto make_value<float>(JSContextRef ctx, float value) -> JSValueRef
    {
        return JSValueMakeNumber(ctx, value);
    }

    template <>
    auto make_value<int>(JSContextRef ctx, int value) -> JSValueRef
    {
        return JSValueMakeNumber(ctx, value);
    }

    template <>
    auto make_value<uint32_t>(JSContextRef ctx, uint32_t value) -> JSValueRef
    {
        return JSValueMakeNumber(ctx, value);
    }

    template <>
    auto make_value<czstring>(JSContextRef ctx, czstring value) -> JSValueRef
    {
        return JSValueMakeString(ctx, UniqueStringRef(value));
    }

    template <>
    auto make_value<Color>(JSContextRef ctx, Color value) -> JSValueRef
    {
        static UniqueStringRef r_prop("r");
        static UniqueStringRef g_prop("g");
        static UniqueStringRef b_prop("b");
        static UniqueStringRef a_prop("a");

        auto color = JSObjectMake(ctx, nullptr, nullptr);
        JSObjectSetProperty(  //
            ctx,
            color,
            r_prop,
            make_value<uint32_t>(ctx, value.r),
            kJSPropertyAttributeDontDelete,
            nullptr);
        JSObjectSetProperty(  //
            ctx,
            color,
            g_prop,
            make_value<uint32_t>(ctx, value.g),
            kJSPropertyAttributeDontDelete,
            nullptr);
        JSObjectSetProperty(  //
            ctx,
            color,
            b_prop,
            make_value<uint32_t>(ctx, value.b),
            kJSPropertyAttributeDontDelete,
            nullptr);
        JSObjectSetProperty(  //
            ctx,
            color,
            a_prop,
            make_value<uint32_t>(ctx, value.a),
            kJSPropertyAttributeDontDelete,
            nullptr);
        return color;
    }

    template <>
    auto make_value<SpriteRef>(JSContextRef ctx, SpriteRef value) -> JSValueRef
    {
        static auto sprite_prototype = [](JSContextRef ctx) {
            auto global = JSContextGetGlobalObject(ctx);

            auto rainbow_prop = JSObjectGetProperty(
                ctx, global, UniqueStringRef("Rainbow"), nullptr);
            R_ASSERT(JSValueIsObject(ctx, rainbow_prop), "Expected an object");

            auto rainbow = JSValueToObject(ctx, rainbow_prop, nullptr);
            auto sprite_prop = JSObjectGetProperty(
                ctx, rainbow, UniqueStringRef("Sprite"), nullptr);
            R_ASSERT(JSValueIsObject(ctx, sprite_prop), "Expected an object");

            auto sprite = JSValueToObject(ctx, sprite_prop, nullptr);
            return JSObjectGetPrototype(ctx, sprite);
        }(ctx);

        auto batch = const_cast<SpriteBatch*>(value.batch());
        auto sprite = std::make_unique<SpriteRef>(*batch, value.index());
        auto object = JSObjectMake(ctx, nullptr, sprite.release());
        JSObjectSetPrototype(ctx, object, sprite_prototype);
        return object;
    }

    template <>
    auto make_value<TextAlignment>(JSContextRef ctx, TextAlignment value)
        -> JSValueRef
    {
        return JSValueMakeNumber(ctx, to_integral_value(value));
    }

    template <>
    auto make_value<Vec2f>(JSContextRef ctx, Vec2f value) -> JSValueRef
    {
        static UniqueStringRef x_prop("x");
        static UniqueStringRef y_prop("y");

        auto v = JSObjectMake(ctx, nullptr, nullptr);
        JSObjectSetProperty(  //
            ctx,
            v,
            x_prop,
            make_value(ctx, value.x),
            kJSPropertyAttributeDontDelete,
            nullptr);
        JSObjectSetProperty(  //
            ctx,
            v,
            y_prop,
            make_value(ctx, value.y),
            kJSPropertyAttributeDontDelete,
            nullptr);
        return v;
    }

    auto register_all(JSGlobalContextRef global_context) -> JSObjectRef;

    template <typename T>
    auto value_to(JSContextRef ctx, JSValueRef value);

    template <typename T>
    auto value_to(JSContextRef ctx, JSValueRef value, T default_value);

    template <>
    auto value_to<bool>(JSContextRef ctx, JSValueRef value)
    {
        return JSValueToBoolean(ctx, value);
    }

    template <>
    auto value_to<float>(JSContextRef ctx, JSValueRef value)
    {
        return static_cast<float>(JSValueToNumber(ctx, value, nullptr));
    }

    template <>
    auto value_to<float>(JSContextRef ctx,
                         JSValueRef value,
                         float default_value)
    {
        return !JSValueIsNumber(ctx, value) ? default_value
                                            : value_to<float>(ctx, value);
    }

    template <>
    auto value_to<int>(JSContextRef ctx, JSValueRef value)
    {
        return static_cast<int>(JSValueToNumber(ctx, value, nullptr));
    }

    template <>
    auto value_to<uint8_t>(JSContextRef ctx, JSValueRef value)
    {
        return static_cast<uint8_t>(JSValueToNumber(ctx, value, nullptr));
    }

    template <>
    auto value_to<uint8_t>(JSContextRef ctx,
                           JSValueRef value,
                           uint8_t default_value)
    {
        return !JSValueIsNumber(ctx, value) ? default_value
                                            : value_to<uint8_t>(ctx, value);
    }

    template <>
    auto value_to<uint32_t>(JSContextRef ctx, JSValueRef value)
    {
        return static_cast<uint32_t>(JSValueToNumber(ctx, value, nullptr));
    }

    template <>
    auto value_to<czstring>(JSContextRef ctx, JSValueRef value)
    {
        thread_local std::string string;

        auto copy = JSValueToStringCopy(ctx, value, nullptr);
        const auto size = JSStringGetMaximumUTF8CStringSize(copy);
        string.resize(size, '\0');
        const auto written = JSStringGetUTF8CString(copy, string.data(), size);
        JSStringRelease(copy);

        string.resize(written - 1);
        return string.c_str();
    }

    template <>
    auto value_to<Animation::Callback>(JSContextRef ctx, JSValueRef value)
    {
        auto object = JSValueToObject(ctx, value, nullptr);
        R_ASSERT(JSObjectIsFunction(ctx, object), "Expected a callable object");
        return Animation::Callback{[ctx, object](
                                       Animation*, AnimationEvent event) {
            const JSValueRef args[]{make_value(ctx, to_integral_value(event))};
            JSObjectCallAsFunction(
                ctx, object, object, array_size(args), args, nullptr);
        }};
    }

    template <>
    auto value_to<Animation::Frames>(JSContextRef ctx, JSValueRef value)
    {
        auto object = JSValueToObject(ctx, value, nullptr);
        UniqueStringRef length_prop("length");
        auto length_value =
            JSObjectGetProperty(ctx, object, length_prop, nullptr);

        auto size = value_to<int>(ctx, length_value);
        auto frames = std::make_unique<Animation::Frame[]>(size + 1);
        for (int i = 0; i < size; ++i)
        {
            auto frame = JSObjectGetPropertyAtIndex(ctx, object, i, nullptr);
            frames[i] = value_to<uint32_t>(ctx, frame);
        }
        return frames;
    }

    template <>
    auto value_to<Color>(JSContextRef ctx, JSValueRef value)
    {
        static UniqueStringRef r_prop("r");
        static UniqueStringRef g_prop("g");
        static UniqueStringRef b_prop("b");
        static UniqueStringRef a_prop("a");

        auto object = JSValueToObject(ctx, value, nullptr);
        auto r = JSObjectGetProperty(ctx, object, r_prop, nullptr);
        auto g = JSObjectGetProperty(ctx, object, g_prop, nullptr);
        auto b = JSObjectGetProperty(ctx, object, b_prop, nullptr);
        auto a = JSObjectGetProperty(ctx, object, a_prop, nullptr);

        return Color{value_to<uint8_t>(ctx, r, 0x00),
                     value_to<uint8_t>(ctx, g, 0x00),
                     value_to<uint8_t>(ctx, b, 0x00),
                     value_to<uint8_t>(ctx, a, 0xff)};
    }

    template <>
    auto value_to<SpriteRef>(JSContextRef ctx, JSValueRef value)
    {
        auto object = JSValueToObject(ctx, value, nullptr);
        return *static_cast<SpriteRef*>(JSObjectGetPrivate(object));
    }

    template <>
    auto value_to<TextAlignment>(JSContextRef ctx, JSValueRef value)
    {
        return static_cast<TextAlignment>(JSValueToNumber(ctx, value, nullptr));
    }

    template <>
    auto value_to<SharedPtr<TextureAtlas>>(JSContextRef ctx, JSValueRef value)
    {
        auto object = JSValueToObject(ctx, value, nullptr);
        return SharedPtr<TextureAtlas>(
            static_cast<TextureAtlas*>(JSObjectGetPrivate(object)));
    }

    template <>
    auto value_to<Vec2f>(JSContextRef ctx, JSValueRef value)
    {
        static UniqueStringRef x_prop("x");
        static UniqueStringRef y_prop("y");

        auto object = JSValueToObject(ctx, value, nullptr);
        auto x = JSObjectGetProperty(ctx, object, x_prop, nullptr);
        auto y = JSObjectGetProperty(ctx, object, y_prop, nullptr);

        return Vec2f{
            value_to<float>(ctx, x, 0.0f), value_to<float>(ctx, y, 0.0f)};
    }
}  // namespace rainbow::jsc

#endif
