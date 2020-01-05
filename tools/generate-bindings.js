// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

// @ts-check
"use strict";

const fs = require("fs");
const path = require("path");
const { makeBanner, writeFile } = require("./import-asset");

const EOL = "\n";

const sourcePath = path.resolve(__dirname, "..", "src");

/**
 * @typedef {
     | "Animation::Callback"
     | "Animation::Frames"
     | "Animation|Label|SpriteBatch"
     | "Animation|Label|SpriteBatch|czstring|int"
     | "Channel"
     | "Channel|Sound"
     | "Channel|undefined"
     | "Color"
     | "Rect"
     | "Sound"
     | "Sound|undefined"
     | "SpriteRef"
     | "TextAlignment"
     | "Texture"
     | "Vec2f"
     | "bool"
     | "czstring"
     | "float"
     | "int"
     | "this"
     | "uint32_t"
     | "undefined"
 * } NativeType
 *
 * @typedef {{
 *   type: NativeType;
 *   name: string;
 *   mustBeMoved?: boolean;
 * }} ParameterInfo
 *
 * @typedef {{
 *   type: string;
 *   name: string;
 * }} PropertyInfo
 *
 * @typedef {{
 *   type: "class";
 *   name: string;
 *   source: string;
 *   sourceName: string;
 *   ctor?: ParameterInfo[] | null;
 *   methods: FunctionInfo[];
 * }} ClassInfo
 *
 * @typedef {{
 *   type: "enum";
 *   name: string;
 *   source: string;
 *   sourceName: string;
 *   values: EnumValueInfo[];
 * }} EnumInfo
 *
 * @typedef {{
 *   name: string;
 *   integralValue: number;
 * }} EnumValueInfo
 *
 * @typedef {{
 *   name: string;
 *   parameters: ParameterInfo[];
 *   returnType?: NativeType;
 * }} FunctionInfo
 *
 * @typedef {{
 *   type: "module";
 *   name: string;
 *   source: string;
 *   sourceName: string;
 *   properties?: PropertyInfo[];
 *   functions?: FunctionInfo[];
 *   types?: (ClassInfo | EnumInfo)[];
 * }} ModuleInfo
 *
 * @typedef {ClassInfo | EnumInfo | ModuleInfo} TypeInfo
 */

/** @type {TypeInfo[]} */
const modules = [
  {
    type: "class",
    name: "Animation",
    source: "Graphics/Animation.h",
    sourceName: "Animation",
    ctor: [
      { type: "SpriteRef", name: "sprite" },
      { type: "Animation::Frames", name: "frames" },
      { type: "uint32_t", name: "fps" },
      { type: "int", name: "delay" }
    ],
    methods: [
      { name: "current_frame", parameters: [], returnType: "uint32_t" },
      { name: "frame_rate", parameters: [], returnType: "uint32_t" },
      { name: "sprite", parameters: [], returnType: "SpriteRef" },
      {
        name: "set_callback",
        parameters: [{ type: "Animation::Callback", name: "callback" }]
      },
      {
        name: "set_delay",
        parameters: [{ type: "int", name: "delay" }]
      },
      {
        name: "set_frame_rate",
        parameters: [{ type: "uint32_t", name: "fps" }]
      },
      {
        name: "set_frames",
        parameters: [
          { type: "Animation::Frames", name: "frames", mustBeMoved: true }
        ]
      },
      {
        name: "set_sprite",
        parameters: [{ type: "SpriteRef", name: "sprite" }]
      },
      {
        name: "jump_to",
        parameters: [{ type: "uint32_t", name: "frame" }]
      },
      { name: "rewind", parameters: [] },
      { name: "start", parameters: [] },
      { name: "stop", parameters: [] }
    ]
  },
  {
    type: "enum",
    name: "AnimationEvent",
    source: "../include/Rainbow/AnimationEvent.h",
    sourceName: "AnimationEvent",
    values: []
  },
  {
    type: "enum",
    name: "ControllerAxis",
    source: "Input/Controller.h",
    sourceName: "ControllerAxis",
    values: []
  },
  {
    type: "enum",
    name: "ControllerButton",
    source: "Input/Controller.h",
    sourceName: "ControllerButton",
    values: []
  },
  {
    type: "class",
    name: "Label",
    source: "Graphics/Label.h",
    sourceName: "Label",
    ctor: [],
    methods: [
      { name: "alignment", parameters: [], returnType: "TextAlignment" },
      { name: "angle", parameters: [], returnType: "float" },
      { name: "color", parameters: [], returnType: "Color" },
      { name: "height", parameters: [], returnType: "float" },
      { name: "length", parameters: [], returnType: "uint32_t" },
      { name: "position", parameters: [], returnType: "Vec2f" },
      { name: "scale", parameters: [], returnType: "float" },
      { name: "text", parameters: [], returnType: "czstring" },
      { name: "width", parameters: [], returnType: "float" },
      {
        name: "set_alignment",
        parameters: [{ type: "TextAlignment", name: "alignment" }]
      },
      { name: "set_color", parameters: [{ type: "Color", name: "color" }] },
      {
        name: "set_font",
        parameters: [{ type: "czstring", name: "font" }]
      },
      {
        name: "set_font_size",
        parameters: [{ type: "int", name: "fontSize" }]
      },
      {
        name: "set_position",
        parameters: [{ type: "Vec2f", name: "position" }]
      },
      { name: "set_rotation", parameters: [{ type: "float", name: "r" }] },
      { name: "set_scale", parameters: [{ type: "float", name: "factor" }] },
      { name: "set_text", parameters: [{ type: "czstring", name: "text" }] },
      { name: "move", parameters: [{ type: "Vec2f", name: "delta" }] }
    ]
  },
  {
    type: "class",
    name: "Sprite",
    source: "Graphics/Sprite.h",
    sourceName: "SpriteRef",
    methods: [
      { name: "angle", parameters: [], returnType: "float" },
      {
        name: "angle",
        parameters: [{ type: "float", name: "r" }],
        returnType: "this"
      },
      { name: "color", parameters: [], returnType: "Color" },
      {
        name: "color",
        parameters: [{ type: "Color", name: "color" }],
        returnType: "this"
      },
      { name: "flip", parameters: [], returnType: "this" },
      { name: "height", parameters: [], returnType: "uint32_t" },
      { name: "hide", parameters: [], returnType: "this" },
      { name: "id", parameters: [], returnType: "int" },
      {
        name: "id",
        parameters: [{ type: "int", name: "id" }],
        returnType: "this"
      },
      { name: "is_flipped", parameters: [], returnType: "bool" },
      { name: "is_hidden", parameters: [], returnType: "bool" },
      { name: "is_mirrored", parameters: [], returnType: "bool" },
      { name: "mirror", parameters: [], returnType: "this" },
      {
        name: "move",
        parameters: [{ type: "Vec2f", name: "dt" }],
        returnType: "this"
      },
      {
        name: "normal",
        parameters: [{ type: "Rect", name: "area" }],
        returnType: "this"
      },
      { name: "pivot", parameters: [], returnType: "Vec2f" },
      {
        name: "pivot",
        parameters: [{ type: "Vec2f", name: "pivot" }],
        returnType: "this"
      },
      { name: "position", parameters: [], returnType: "Vec2f" },
      {
        name: "position",
        parameters: [{ type: "Vec2f", name: "position" }],
        returnType: "this"
      },
      {
        name: "rotate",
        parameters: [{ type: "float", name: "r" }],
        returnType: "this"
      },
      { name: "scale", parameters: [], returnType: "Vec2f" },
      {
        name: "scale",
        parameters: [{ type: "Vec2f", name: "factor" }],
        returnType: "this"
      },
      { name: "show", parameters: [], returnType: "this" },
      {
        name: "texture",
        parameters: [{ type: "Rect", name: "area" }],
        returnType: "this"
      },
      { name: "width", parameters: [], returnType: "uint32_t" }
    ]
  },
  {
    type: "class",
    name: "SpriteBatch",
    source: "Graphics/SpriteBatch.h",
    sourceName: "SpriteBatch",
    ctor: [{ type: "uint32_t", name: "count" }],
    methods: [
      { name: "is_visible", parameters: [], returnType: "bool" },
      {
        name: "set_normal",
        parameters: [{ type: "Texture", name: "texture" }]
      },
      {
        name: "set_texture",
        parameters: [{ type: "Texture", name: "texture" }]
      },
      {
        name: "set_visible",
        parameters: [{ type: "bool", name: "visible" }]
      },
      { name: "clear", parameters: [] },
      {
        name: "create_sprite",
        parameters: [
          { type: "uint32_t", name: "width" },
          { type: "uint32_t", name: "height" }
        ],
        returnType: "SpriteRef"
      },
      { name: "erase", parameters: [{ type: "uint32_t", name: "i" }] },
      {
        name: "find_sprite_by_id",
        parameters: [{ type: "int", name: "id" }],
        returnType: "SpriteRef"
      },
      {
        name: "swap",
        parameters: [
          { type: "SpriteRef", name: "a" },
          { type: "SpriteRef", name: "b" }
        ]
      }
    ]
  },
  {
    type: "enum",
    name: "TextAlignment",
    source: "../include/Rainbow/TextAlignment.h",
    sourceName: "TextAlignment",
    values: []
  },
  {
    type: "class",
    name: "Texture",
    source: "Graphics/Texture.h",
    sourceName: "graphics::Texture",
    ctor: [{ type: "czstring", name: "path" }],
    methods: []
  },
  {
    type: "enum",
    name: "VirtualKey",
    source: "Input/VirtualKey.h",
    sourceName: "VirtualKey",
    values: []
  },
  {
    type: "module",
    name: "Audio",
    source: "Audio/Mixer.h",
    sourceName: "Audio",
    functions: [
      {
        name: "load_sound",
        parameters: [{ type: "czstring", name: "path" }],
        returnType: "Sound|undefined"
      },
      {
        name: "load_stream",
        parameters: [{ type: "czstring", name: "path" }],
        returnType: "Sound|undefined"
      },
      {
        name: "release",
        parameters: [{ type: "Sound", name: "sound" }],
        returnType: "undefined"
      },
      {
        name: "is_paused",
        parameters: [{ type: "Channel", name: "channel" }],
        returnType: "bool"
      },
      {
        name: "is_playing",
        parameters: [{ type: "Channel", name: "channel" }],
        returnType: "bool"
      },
      {
        name: "set_loop_count",
        parameters: [
          { type: "Channel", name: "channel" },
          { type: "int", name: "count" }
        ]
      },
      {
        name: "set_volume",
        parameters: [
          { type: "Channel", name: "channel" },
          { type: "float", name: "volume" }
        ]
      },
      {
        name: "set_world_position",
        parameters: [
          { type: "Channel", name: "channel" },
          { type: "Vec2f", name: "position" }
        ]
      },
      { name: "pause", parameters: [{ type: "Channel", name: "channel" }] },
      {
        name: "play",
        parameters: [{ type: "Channel|Sound", name: "audial" }],
        returnType: "Channel|undefined"
      },
      { name: "stop", parameters: [{ type: "Channel", name: "channel" }] }
    ],
    types: [
      {
        type: "class",
        name: "Channel",
        source: "Audio/Mixer.h",
        sourceName: "Channel",
        methods: []
      },
      {
        type: "class",
        name: "Sound",
        source: "Audio/Mixer.h",
        sourceName: "Sound",
        methods: []
      }
    ]
  },
  {
    type: "module",
    name: "Input",
    source: "Input/Input.h",
    sourceName: "Input",
    properties: [
      { type: "Float64Array", name: "acceleration" },
      {
        type: "ReadonlyArray<Readonly<ControllerState>>",
        name: "controllers"
      },
      { type: "Int8Array", name: "keysDown" },
      {
        type: "ReadonlyArray<Readonly<Pointer>>",
        name: "pointersDown"
      },
      {
        type: "ReadonlyArray<Readonly<Pointer>>",
        name: "pointersMoved"
      },
      {
        type: "ReadonlyArray<Readonly<Pointer>>",
        name: "pointersUp"
      }
    ]
  },
  {
    type: "module",
    name: "RenderQueue",
    source: "Graphics/RenderQueue.h",
    sourceName: "RenderQueue",
    functions: [
      {
        name: "add",
        parameters: [{ type: "Animation|Label|SpriteBatch", name: "obj" }]
      },
      {
        name: "disable",
        parameters: [
          { type: "Animation|Label|SpriteBatch|czstring|int", name: "obj" }
        ]
      },
      {
        name: "enable",
        parameters: [
          { type: "Animation|Label|SpriteBatch|czstring|int", name: "obj" }
        ]
      },
      {
        name: "insert",
        parameters: [
          { type: "uint32_t", name: "position" },
          { type: "Animation|Label|SpriteBatch", name: "obj" }
        ]
      },
      {
        name: "erase",
        parameters: [
          { type: "Animation|Label|SpriteBatch|czstring|int", name: "obj" }
        ]
      },
      {
        name: "set_tag",
        parameters: [
          { type: "Animation|Label|SpriteBatch", name: "obj" },
          { type: "czstring", name: "tag" }
        ]
      }
    ]
  }
];

/**
 * Generates C++ bindings.
 * @param {TypeInfo[]} typeInfo
 * @returns {string}
 */
function generateCppBindings(typeInfo) {
  /** @typedef {{ [key: string]: FunctionInfo[] }} FunctionsMap */

  /** @type {(info: ParameterInfo) => string} */
  const toCppType = ({ type }) => {
    switch (type) {
      case "Texture":
        return "graphics::Texture*";
      default:
        return type;
    }
  };

  /** @type {(parameters: ParameterInfo[]) => string} */
  const joinTypeParams = parameters => parameters.map(toCppType).join(", ");

  /** @type {(typeName: string, method: FunctionInfo, indent?: string) => string} */
  const registerMethod = (
    typeName,
    { name, parameters, returnType },
    indent = "    "
  ) => {
    const typeParams = joinTypeParams(parameters);
    const result = !returnType || returnType === "this" ? "" : "auto result = ";
    return [
      `${indent}    auto obj = duk::push_this<${typeName}>(ctx);`,
      ...(typeParams
        ? [`${indent}    auto args = duk::get_args<${typeParams}>(ctx);`]
        : []),
      `${indent}    ${result}obj->${name}(${parameters
        .map((p, i) => {
          const get = `std::get<${i}>(args)`;
          return p.mustBeMoved ? `std::move(${get})` : get;
        })
        .join(", ")});`,
      ...(result ? [`${indent}    duk::push(ctx, result);`] : []),
      `${indent}    return ${result || returnType === "this" ? 1 : 0};`
    ].join(EOL);
  };

  /** @type {(typeName: string, methods: FunctionInfo[]) => string} */
  const registerOverloadedMethods = (typeName, methods) => {
    return [
      `                switch (duk_get_top(ctx))`,
      `                {`,
      ...methods.map(info => {
        return [
          `                    case ${info.parameters.length}: {`,
          registerMethod(typeName, info, "                    "),
          `                    }`
        ].join(EOL);
      }),
      `                    default:`,
      `                        duk_push_error_object(ctx, DUK_ERR_SYNTAX_ERROR, "invalid number of arguments");`,
      `                        return DUK_RET_SYNTAX_ERROR;`,
      `                }`
    ].join(EOL);
  };

  /** @type {(typeName: string, functionsMap: FunctionsMap) => (name: string) => string} */
  const registerMethods = (typeName, functionsMap) => name => {
    const functions = functionsMap[name];
    return [
      `        duk_push_c_function(`,
      `            ctx,`,
      `            [](duk_context* ctx) -> duk_ret_t {`,
      functions.length === 1
        ? registerMethod(typeName, functions[0], "            ")
        : registerOverloadedMethods(typeName, functions),
      `            },`,
      `            ${
        functions.length === 1 ? functions[0].parameters.length : "DUK_VARARGS"
      });`,
      `        duk::put_prop_literal(ctx, -2, "${toCamelCase(name)}");`
    ].join(EOL);
  };

  /** @type {(reg: FunctionsMap, info: FunctionInfo) => FunctionsMap} */
  const remapFunctions = (reg, info) => {
    if (!reg[info.name]) {
      reg[info.name] = [info];
    } else {
      reg[info.name].push(info);
    }
    return reg;
  };

  /** @type {(typeInfo: ClassInfo) => string[]} */
  const defineClass = ({ ctor, methods, name, sourceName }) => {
    const methodMap = methods.reduce(
      remapFunctions,
      /** @type {FunctionsMap} */ ({})
    );
    return [
      ctor
        ? `    duk::push_constructor<${sourceName}${
            ctor.length === 0 ? "" : `, ${joinTypeParams(ctor)}`
          }>(ctx);`
        : "    duk_push_bare_object(ctx);",
      `    duk::put_prototype<${sourceName}, Allocation::${
        ctor ? "HeapAllocated" : "NoHeap"
      }>(ctx, [](duk_context* ctx) {`,
      ...Object.keys(methodMap).map(registerMethods(sourceName, methodMap)),
      `        duk::push_literal(ctx, "Rainbow.${name}");`,
      "        duk::put_prop_literal(ctx, -2, DUKR_WELLKNOWN_SYMBOL_TOSTRINGTAG);",
      "    });"
    ];
  };

  /** @type {(sourceName: string) => (value: EnumValueInfo) => string} */
  const registerEnumValue = sourceName => ({ name }) => {
    return `    duk_push_int(ctx, to_underlying_type(${sourceName}::${name}));
    duk::put_prop_literal(ctx, obj_idx, "${name}");`;
  };

  /** @type {(typeInfo: EnumInfo) => string[]} */
  const defineEnum = ({ sourceName, values }) => {
    return [
      "    const auto obj_idx = duk_push_bare_object(ctx);",
      ...values.map(registerEnumValue(sourceName))
    ];
  };

  /** @type {(typeInfo: TypeInfo) => string} */
  const defineModule = typeInfo => {
    const sourceName = typeInfo.sourceName;
    return [
      "",
      "template <>",
      `void rainbow::duk::register_module<rainbow::${sourceName}>(duk_context* ctx, duk_idx_t rainbow)`,
      "{",
      .../** @type {() => string[]} */ (() => {
        switch (typeInfo.type) {
          case "class":
            return defineClass(typeInfo);
          case "enum":
            return defineEnum(typeInfo);
          case "module":
            return [];
        }
      })(),
      "    duk_freeze(ctx, -1);",
      `    duk::put_prop_literal(ctx, rainbow, "${typeInfo.name}");`,
      "}"
    ].join(EOL);
  };

  /** @type {(typeInfo: TypeInfo[], extraIncludes?: string[]) => string[]} */
  const includeHeaders = (typeInfo, extraIncludes = []) => {
    return typeInfo
      .map(type => type.source)
      .concat(extraIncludes)
      .sort()
      .reduce(
        (includes, p, i, typeInfo) => {
          if (i === 0 || p !== typeInfo[i - 1]) {
            const header =
              p.lastIndexOf("../include/Rainbow/", 0) === 0
                ? `<${p.replace("../include/", "")}>`
                : `"${p}"`;
            includes.push(`#include ${header}`);
          }
          return includes;
        },
        /** @type {string[]} */ ([])
      );
  };

  /** @type {(typeInfo: TypeInfo) => string} */
  const registerModule = ({ sourceName }) => {
    const typeName = sourceName.replace(/rainbow::/g, "");
    return `        duk::register_module<${typeName}>(ctx, obj_idx);`;
  };

  return [
    makeBanner(__filename),
    "",
    "#ifndef SCRIPT_JAVASCRIPT_MODULES_H_",
    "#define SCRIPT_JAVASCRIPT_MODULES_H_",
    "",
    ...includeHeaders(typeInfo, [
      "Common/TypeCast.h",
      "Common/TypeInfo.h",
      "Script/JavaScript/Helper.h"
    ]),
    "",
    "#ifdef __GNUC__",
    "#    pragma GCC diagnostic push",
    '#    pragma GCC diagnostic ignored "-Wold-style-cast"',
    "#endif",
    "",
    "// clang-format off",
    ...typeInfo.filter(({ type }) => type !== "module").map(defineModule),
    "",
    "namespace rainbow::duk",
    "{",
    "    void register_all_modules(duk_context* ctx, duk_idx_t obj_idx)",
    "    {",
    ...typeInfo.filter(({ type }) => type !== "module").map(registerModule),
    "    }",
    "}",
    "",
    "// clang-format on",
    "",
    "#ifdef __GNUC__",
    "#    pragma GCC diagnostic pop",
    "#endif",
    "",
    "#endif",
    ""
  ].join(EOL);
}

/**
 * Generates TypeScript declaration file.
 * @param {TypeInfo[]} typeInfo
 * @returns {string}
 */
function generateTypeScriptDeclaration(typeInfo) {
  /** @type {(typeInfo: ClassInfo, indent: string) => string} */
  const declareClass = ({ ctor, methods, name }, indent) => {
    const [exportType, accessibility, ctorDecl, endline] = !ctor
      ? [`type ${name} =`, "", [], ";"]
      : [
          `class ${name}`,
          " private",
          [`${indent}    constructor(${joinParams(ctor)});`],
          ""
        ];
    return [
      `${indent}  export ${exportType} {`,
      `${indent}   ${accessibility} readonly $type: "Rainbow.${name}";`,
      ...ctorDecl,
      ...methods.map(method => declareFunction(method, name, indent)),
      `${indent}  }${endline}`
    ].join(EOL);
  };

  /** @type {(typeInfo: EnumInfo, indent: string) => string} */
  const declareEnum = ({ name, values }, indent) => {
    return [
      `${indent}  export enum ${name} {`,
      ...values.map(defineEnumValue(indent)),
      `${indent}  }`
    ].join(EOL);
  };

  /** @type {(func: FunctionInfo, className: string, indent: string) => string} */
  const declareFunction = (func, className, indent) => {
    const keyword = className ? "" : "function ";
    const name = toCamelCase(func.name);
    const params = joinParams(func.parameters);
    const returnType =
      func.returnType === "this"
        ? className
        : toTypeScriptType(func.returnType);
    return `${indent}    ${keyword}${name}(${params}): ${returnType};`;
  };

  /** @type {(typeInfo: ModuleInfo, indent: string) => string} */
  const declareModule = ({ functions, name, properties, types }, indent) => {
    return [
      `${indent}  export namespace ${name} {`,
      ...map(properties, declareProp(indent)),
      ...map(functions, f => declareFunction(f, "", indent)),
      ...map(types, t => declareType(t, "  ")),
      `${indent}  }`
    ].join(EOL);
  };

  /** @type {(indent: string) => (prop: PropertyInfo) => string} */
  const declareProp = indent => prop =>
    `${indent}    const ${prop.name}: ${prop.type};`;

  /** @type {(t: TypeInfo, indent?: string) => string} */
  const declareType = (t, indent = "") => {
    switch (t.type) {
      case "class":
        return declareClass(t, indent);
      case "enum":
        return declareEnum(t, indent);
      case "module":
        return declareModule(t, indent);
    }
  };

  /** @type {(indent: string) => (value: EnumValueInfo) => string} */
  const defineEnumValue = indent => value =>
    `${indent}    ${value.name} = ${value.integralValue},`;

  /** @type {(parameters: ParameterInfo[]) => string} */
  const joinParams = parameters => {
    return parameters
      .map(p => `${p.name}: ${toTypeScriptType(p.type)}`)
      .join(", ");
  };

  /** @type {(type?: NativeType) => string} */
  const toTypeScriptType = type => {
    return (type || "")
      .split("|")
      .map(t => {
        switch (t) {
          case "Animation::Callback":
            return "(animation: Animation, event: AnimationEvent) => void";
          case "Animation::Frames":
            return "Rect[]";
          case "SpriteRef":
            return "Sprite";
          case "bool":
            return "boolean";
          case "czstring":
            return "string";
          case "float":
            return "number";
          case "int":
            return "number";
          case "uint32_t":
            return "number";
          default:
            return t || "void";
        }
      })
      .sort()
      .join(" | ");
  };

  return `${makeBanner(__filename)}

/* eslint-disable no-dupe-class-members, no-unused-vars */

declare namespace Duktape {
  const version: number;
  const env: string;

  function fin(o: {}, finalizer?: () => void): (() => void) | undefined;
  function enc(format: "base64" | "hex" | "jc" | "jx", ...values: any[]): string;
  function dec(format: "base64" | "hex" | "jc" | "jx", value: string): Buffer | string;
  function info(value: any): {};
  function act(index: number): {};
  function gc(flags?: number): void;
  function compact(o: {}): void;

  let errCreate: ((e: Error) => Error) | undefined;
  let errThrow: ((e: Error) => Error) | undefined;

  class Thread {
    static current(): Thread;
    static resume(thread: Thread, value?: any, thrown?: boolean): any;
    static yield(value?: any, thrown?: boolean): any;

    private readonly $type: "Duktape.Thread";

    constructor(yielder: (value?: any) => any);
  }
}

declare namespace Rainbow {
  export type Color = {
    r: number;
    g: number;
    b: number;
    a: number;
  };

  export type ControllerState = {
    id: number;
    buttons: Int8Array;
    axes: Int32Array;
  };

  export type Pointer = {
    hash: number;
    x: number;
    y: number;
    timestamp: number;
  };

  export type Rect = {
    left: number;
    bottom: number;
    width: number;
    height: number;
  };

  export type Vec2f = {
    x: number;
    y: number;
  };

${typeInfo.map(m => declareType(m)).join(`${EOL}${EOL}`)}
}
`;
}

/**
 * @template T, U
 * @param {T[]=} array
 * @param {(value: T, index: number, array: T[]) => U} f
 * @returns {U[]}
 */
function map(array, f) {
  return !array ? [] : array.map(f);
}

/**
 * Pre-processes specified type information.
 * @param {TypeInfo} info
 * @returns {TypeInfo}
 */
function preprocess(info) {
  if (info.type !== "enum") {
    return info;
  }

  const sourceFilePath = path.join(
    sourcePath,
    info.source.replace(/[/\\]+/g, path.sep)
  );

  console.log(`${info.name} <- ${sourceFilePath}`);

  const data = fs.readFileSync(sourceFilePath);
  const marker = `${info.type}[\\s\\w]+?${info.sourceName}`;
  const source = data.toString("utf8");
  if (!new RegExp(marker, "g").test(source)) {
    console.error(`'${info.sourceName}' was not found in '${sourceFilePath}'`);
    return info;
  }

  const re = new RegExp(`${marker}.*?{(.*?)};`, "g");
  const matches = re.exec(source.replace(/\s+/g, " "));
  if (!matches) {
    console.error(`Failed to extract values for '${info.sourceName}'`);
    return info;
  }

  let count = -1;
  return {
    ...info,
    values: matches[1].split(",").map(value => {
      const parts = value.split("=");
      count = parts[1] ? parseInt(parts[1]) : count + 1;
      return {
        name: parts[0].trim(),
        integralValue: count
      };
    })
  };
}

/**
 * Returns a copy of the string in camelCase.
 * @param {string} str
 */
function toCamelCase(str) {
  return str
    .split("")
    .reduce(
      (arr, char, index) => {
        if (char === "_") {
          return arr;
        }

        if (str[index - 1] === "_") {
          arr.push(char.toUpperCase());
        } else {
          arr.push(char);
        }
        return arr;
      },
      /** @type {string[]} */ ([])
    )
    .join("");
}

const typeInfo = modules.map(preprocess);
[
  {
    generate: generateCppBindings,
    output: path.join(sourcePath, "Script", "JavaScript", "Modules.g.h")
  },
  {
    generate: generateTypeScriptDeclaration,
    output: path.join(sourcePath, "..", "js", "index.d.ts")
  }
].forEach(({ generate, output }) => writeFile(output, generate(typeInfo)));
