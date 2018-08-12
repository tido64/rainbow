set(DUK_TARGET_VERSION 2.3.0)
set(DUK_URL_HASH ed6e2f3ab2061628a7aeee27ccff16538ba6a151480cccf2e846bd061e45afe8)

download_library(
    duktape-source
    https://github.com/svaarala/duktape/releases/download/v${DUK_TARGET_VERSION}/duktape-${DUK_TARGET_VERSION}.tar.xz
    ${DUK_URL_HASH}
)

ExternalProject_Get_Property(duktape-source SOURCE_DIR)
set(DUK_SOURCE_FILES
    ${SOURCE_DIR}/src/duktape.c
    ${SOURCE_DIR}/extras/module-node/duk_module_node.c
)
set_source_files_properties(${DUK_SOURCE_FILES} PROPERTIES GENERATED TRUE)

add_library(duktape STATIC ${DUK_SOURCE_FILES})
target_compile_definitions(
    duktape
    PRIVATE
        DUK_USE_ARRAY_FASTPATH=1
        DUK_USE_ARRAY_PROP_FASTPATH=1
        DUK_USE_BASE64_FASTPATH=1
        DUK_USE_FAST_REFCOUNT_DEFAULT=1
        DUK_USE_HEX_FASTPATH=1
        DUK_USE_IDCHAR_FASTPATH=1
        DUK_USE_JSON_DECNUMBER_FASTPATH=1
        DUK_USE_JSON_DECSTRING_FASTPATH=1
        DUK_USE_JSON_EATWHITE_FASTPATH=1
        DUK_USE_JSON_QUOTESTRING_FASTPATH=1
        DUK_USE_SYMBOL_BUILTIN=1
        $<$<CONFIG:Debug>:DUK_USE_INTERRUPT_COUNTER=1>
        $<$<CONFIG:Debug>:DUK_USE_DEBUGGER_DUMPHEAP=1>
        $<$<CONFIG:Debug>:DUK_USE_DEBUGGER_INSPECT=1>
        $<$<CONFIG:Debug>:DUK_USE_DEBUGGER_PAUSE_UNCAUGHT=1>
        $<$<CONFIG:Debug>:DUK_USE_DEBUGGER_SUPPORT=1>
        $<$<CONFIG:Debug>:DUK_USE_DEBUGGER_THROW_NOTIFY=1>
        $<$<NOT:$<CONFIG:Debug>>:DUK_USE_VALSTACK_UNSAFE=1>
)
target_include_directories(
    duktape
    PUBLIC
        ${SOURCE_DIR}/src
        ${SOURCE_DIR}/extras/module-node
)

add_dependencies(duktape duktape-source)
target_link_libraries(rainbow duktape)
