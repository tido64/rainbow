// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

#include "Script/V8/JSInstance.h"

#include <optional>

#ifdef __GNUC__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include <libplatform/libplatform.h>
#ifdef __GNUC__
#    pragma GCC diagnostic pop
#endif

#include "Common/Data.h"
#include "Common/Logging.h"
#include "FileSystem/File.h"
#include "Script/V8/Bindings.g.h"

using rainbow::Data;
using rainbow::JSInstance;

namespace
{
    auto log_exception(v8::Local<v8::Context> context,
                       const v8::TryCatch& try_catch)
    {
        auto stack_trace = try_catch.StackTrace(context);
        auto error = stack_trace.IsEmpty() ? try_catch.Exception()
                                           : stack_trace.ToLocalChecked();
        LOGF("JS: %s", *v8::String::Utf8Value{context->GetIsolate(), error});
    }

    bool call_global(v8::Local<v8::Context> context,
                     v8::Local<v8::Function> func,
                     int argc,
                     v8::Local<v8::Value> argv[])
    {
        v8::TryCatch try_catch(context->GetIsolate());

        v8::Local<v8::Value> result;
        if (!func->Call(context, context->Global(), argc, argv)
                 .ToLocal(&result))
        {
            log_exception(context, try_catch);
            return false;
        }

        return true;
    }

    auto get_function(v8::Local<v8::Context> context, const char* name)
        -> std::optional<v8::Local<v8::Function>>
    {
        auto func_name =
            v8::String::NewFromUtf8(
                context->GetIsolate(), name, v8::NewStringType::kNormal)
                .ToLocalChecked();
        v8::Local<v8::Value> value;
        if (!context->Global()->Get(context, func_name).ToLocal(&value) ||
            !value->IsFunction())
        {
            return std::nullopt;
        }

        return std::make_optional(v8::Local<v8::Function>::Cast(value));
    }
}  // namespace

JSInstance::JSInstance()
    : allocator_(v8::ArrayBuffer::Allocator::NewDefaultAllocator()),
      platform_(v8::platform::NewDefaultPlatform())
{
    v8::V8::InitializePlatform(platform_.get());
    v8::V8::Initialize();

    v8::Isolate::CreateParams create_params;
    create_params.array_buffer_allocator = allocator_.get();
    isolate_ = v8::Isolate::New(create_params);
    isolate_->Enter();

    auto index_js = filesystem::main_script();
    if (index_js == nullptr)
    {
        constexpr const char* const kEntryPoints[]{
            "index.js",
            "index.bs.js",
            "main.jsbundle",
        };

        auto end = std::end(kEntryPoints);
        auto i = std::find_if(std::begin(kEntryPoints), end, [](auto&& path) {
            return filesystem::exists(path);
        });
        if (i == end)
        {
            LOGF("No script was found at these locations:");
            auto assets_path = filesystem::assets_path() == nullptr
                                   ? "assets"
                                   : filesystem::assets_path();
            for (auto&& path : kEntryPoints)
            {
                LOGF("    %s%s%s",
                     assets_path,
                     filesystem::path_separator(),
                     path);
            }
            // terminate(ErrorCode::ScriptNotFound);
            return;
        }

        index_js = *i;
    }

    v8::HandleScope handle_scope(isolate_);

    auto context = v8::Context::New(isolate_);
    context_.Set(isolate_, context);

    v8::Context::Scope context_scope(context);
    v8::TryCatch try_catch(isolate_);

    v8::ScriptOrigin origin(
        v8::String::NewFromUtf8(isolate_, index_js, v8::NewStringType::kNormal)
            .ToLocalChecked());

    v8::Local<v8::String> source;
    v8::Local<v8::Script> script;
    v8::Local<v8::Value> result;
    std::optional<v8::Local<v8::Function>> update_func;
    std::optional<v8::Local<v8::Function>> init_func;

    {
        const auto data = File::read(index_js, FileType::Asset);
        if (!v8::String::NewFromUtf8(isolate_,
                                     data.as<char*>(),
                                     v8::NewStringType::kNormal,
                                     data.size())
                 .ToLocal(&source))
        {
            goto error;
        }
    }

    if (!v8::Script::Compile(context, source, &origin).ToLocal(&script))
        goto error;

    if (!script->Run(context).ToLocal(&result))
        goto error;

    update_func = get_function(context, "update");
    if (!update_func)
        goto error;

    update_.Set(isolate_, *update_func);

    init_func = get_function(context, "init");
    if (!init_func)
        goto error;

    init_.Set(isolate_, *init_func);

    js::set_module(
        context, context->Global(), "Rainbow", js::generate_bindings(context));
    return;

error:
    log_exception(context, try_catch);
}

JSInstance::~JSInstance()
{
    isolate_->Dispose();
    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}

void JSInstance::init_impl(const Vec2i& screen_size)
{
    v8::HandleScope handle_scope(isolate_);

    auto context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Value> argv[]{
        v8::Int32::New(isolate_, screen_size.x),
        v8::Int32::New(isolate_, screen_size.y),
    };
    call_global(context, init_.Get(isolate_), array_size(argv), argv);
}

void JSInstance::update_impl(uint64_t dt)
{
    v8::HandleScope handle_scope(isolate_);

    auto context = context_.Get(isolate_);
    v8::Context::Scope context_scope(context);

    v8::Local<v8::Value> argv[]{v8::Uint32::New(isolate_, dt)};
    call_global(context, update_.Get(isolate_), array_size(argv), argv);
}

void JSInstance::on_memory_warning_impl()
{
    isolate_->LowMemoryNotification();
}
