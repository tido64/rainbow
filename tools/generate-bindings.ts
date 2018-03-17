import * as fs from "fs";
import * as path from "path";

const sourcePath = path.resolve(path.dirname(process.argv[1]), "../src");
const outputBindingsPath = path.join(
  sourcePath,
  "Script",
  "JavaScript",
  "Modules.g.h"
);
const outputDeclarationPath = path.join(sourcePath, "..", "js", "index.d.ts");

const modules: TypeInfo[] = [
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
        parameters: [{ type: "Animation::Frames", name: "frames" }]
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
      { name: "color", parameters: [], returnType: "Color" },
      { name: "height", parameters: [], returnType: "uint32_t" },
      { name: "id", parameters: [], returnType: "int" },
      { name: "is_flipped", parameters: [], returnType: "bool" },
      { name: "is_hidden", parameters: [], returnType: "bool" },
      { name: "is_mirrored", parameters: [], returnType: "bool" },
      { name: "pivot", parameters: [], returnType: "Vec2f" },
      { name: "position", parameters: [], returnType: "Vec2f" },
      { name: "scale", parameters: [], returnType: "Vec2f" },
      { name: "width", parameters: [], returnType: "uint32_t" },
      { name: "set_color", parameters: [{ type: "Color", name: "color" }] },
      { name: "set_id", parameters: [{ type: "int", name: "id" }] },
      { name: "set_normal", parameters: [{ type: "uint32_t", name: "id" }] },
      { name: "set_pivot", parameters: [{ type: "Vec2f", name: "pivot" }] },
      {
        name: "set_position",
        parameters: [{ type: "Vec2f", name: "position" }]
      },
      { name: "set_rotation", parameters: [{ type: "float", name: "r" }] },
      { name: "set_scale", parameters: [{ type: "Vec2f", name: "factor" }] },
      { name: "set_texture", parameters: [{ type: "uint32_t", name: "id" }] },
      { name: "flip", parameters: [] },
      { name: "hide", parameters: [] },
      { name: "mirror", parameters: [] },
      { name: "move", parameters: [{ type: "Vec2f", name: "dt" }] },
      { name: "rotate", parameters: [{ type: "float", name: "r" }] },
      { name: "show", parameters: [] }
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
        parameters: [{ type: "SharedPtr<TextureAtlas>", name: "texture" }]
      },
      {
        name: "set_texture",
        parameters: [{ type: "SharedPtr<TextureAtlas>", name: "texture" }]
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
    source: "Graphics/TextureAtlas.h",
    sourceName: "SharedPtr<rainbow::TextureAtlas>",
    ctor: [{ type: "czstring", name: "path" }],
    methods: [
      {
        name: "add_region",
        parameters: [
          { type: "int", name: "x" },
          { type: "int", name: "y" },
          { type: "int", name: "width" },
          { type: "int", name: "height" }
        ],
        returnType: "uint32_t"
      },
      { name: "trim", parameters: [] }
    ]
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
        type:
          "ReadonlyArray<Readonly<{ id: number, buttons: Int8Array, axes: Int32Array }>>",
        name: "controllers"
      },
      { type: "Int8Array", name: "keysDown" },
      {
        type:
          "ReadonlyArray<Readonly<{ hash: number, x: number, y: number, timestamp: number }>>",
        name: "pointersDown"
      },
      {
        type:
          "ReadonlyArray<Readonly<{ hash: number, x: number, y: number, timestamp: number }>>",
        name: "pointersMoved"
      },
      {
        type:
          "ReadonlyArray<Readonly<{ hash: number, x: number, y: number, timestamp: number }>>",
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

interface ClassInfo {
  type: "class";
  name: string;
  source: string;
  sourceName: string;
  ctor?: ParameterInfo[] | null;
  methods: FunctionInfo[];
}

interface EnumInfo {
  type: "enum";
  name: string;
  source: string;
  sourceName: string;
  values: EnumValueInfo[];
}

interface EnumValueInfo {
  name: string;
  integralValue: number;
}

interface FunctionInfo {
  name: string;
  parameters: ParameterInfo[];
  returnType?: NativeType;
}

interface ModuleInfo {
  type: "module";
  name: string;
  source: string;
  sourceName: string;
  properties?: PropertyInfo[];
  functions?: FunctionInfo[];
  types?: (ClassInfo | EnumInfo)[];
}

interface ParameterInfo {
  type: NativeType;
  name: string;
}

interface PropertyInfo {
  type: string;
  name: string;
}

type NativeType =
  | "Animation::Callback"
  | "Animation::Frames"
  | "Animation|Label|SpriteBatch"
  | "Animation|Label|SpriteBatch|czstring|int"
  | "Channel"
  | "Channel|Sound"
  | "Channel|undefined"
  | "Color"
  | "SharedPtr<TextureAtlas>"
  | "Sound"
  | "Sound|undefined"
  | "SpriteRef"
  | "TextAlignment"
  | "Vec2f"
  | "bool"
  | "czstring"
  | "float"
  | "int"
  | "uint32_t"
  | "undefined";

type TypeInfo = ClassInfo | EnumInfo | ModuleInfo;

function generateCppBindings(typeInfo: TypeInfo[]): string {
  const joinTypeParams = (parameters: ParameterInfo[]): string => {
    return parameters.map(p => p.type.replace(/&+$/g, "")).join(", ");
  };
  return [
    "// This file was generated by `tools/generate-bindings.ts`. DO NOT MODIFY!",
    "//",
    "// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen",
    "// Distributed under the MIT License.",
    "// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)",
    "",
    "#ifndef SCRIPT_JAVASCRIPT_MODULES_H_",
    "#define SCRIPT_JAVASCRIPT_MODULES_H_",
    "",
    '#include "Common/TypeInfo.h"',
    ...typeInfo
      .map(
        type =>
          `#include ${
            type.source.lastIndexOf("../include/Rainbow/", 0) === 0
              ? `<${type.source.replace("../include/", "")}>`
              : `"${type.source}"`
          }`
      )
      .sort()
      .reduce<string[]>((arr, header) => {
        const length = arr.length;
        return length > 0 && header == arr[length - 1] ? arr : [...arr, header];
      }, []),
    '#include "Script/JavaScript/Helper.h"',
    "",
    "// clang-format off",
    ...typeInfo.filter(type => type.type !== "module").map(type => {
      const sourceName = type.sourceName;
      return [
        "",
        "template <>",
        `void rainbow::duk::register_module<rainbow::${sourceName}>(duk_context* ctx, duk_idx_t rainbow)`,
        "{",
        ...((): string[] => {
          switch (type.type) {
            case "class":
              return [
                type.ctor
                  ? `    duk::push_constructor<${sourceName}${
                      type.ctor.length === 0
                        ? ""
                        : `, ${joinTypeParams(type.ctor)}`
                    }>(ctx);`
                  : "    duk_push_bare_object(ctx);",
                `    duk::put_prototype<${sourceName}, Allocation::${
                  type.ctor ? "HeapAllocated" : "NoHeap"
                }>(ctx, [](duk_context* ctx) {`,
                ...type.methods
                  .map(method => {
                    const typeParams = joinTypeParams(method.parameters);
                    const result = !method.returnType ? "" : "auto result = ";
                    return [
                      "        duk_push_c_function(",
                      "            ctx,",
                      "            [](duk_context* ctx) -> duk_ret_t {",
                      `                auto obj = duk::push_this<${sourceName}>(ctx);`,
                      ...(typeParams
                        ? [
                            `                auto args = duk::get_args<${typeParams}>(ctx);`
                          ]
                        : []),
                      `                ${result}obj->${
                        method.name
                      }(${method.parameters
                        .map((p, i) => `duk::forward(std::get<${i}>(args))`)
                        .join(", ")});`,
                      ...(result
                        ? [`                duk::push(ctx, result);`]
                        : []),
                      `                return ${result ? 1 : 0};`,
                      "            },",
                      `            ${method.parameters.length});`,
                      `        duk::put_prop_literal(ctx, -2, "${toCamelCase(
                        method.name
                      )}");`
                    ];
                  })
                  .reduce<string[]>((arr, lines) => {
                    return [...arr, ...lines];
                  }, []),
                `        duk::push_literal(ctx, "Rainbow.${type.name}");`,
                "        duk::put_prop_literal(ctx, -2, DUKR_WELLKNOWN_SYMBOL_TOSTRINGTAG);",
                "    });"
              ];
            case "enum":
              return [
                "    const auto obj_idx = duk_push_bare_object(ctx);",
                ...type.values
                  .map(value => [
                    `    duk_push_int(ctx, static_cast<int>(${sourceName}::${
                      value.name
                    }));`,
                    `    duk::put_prop_literal(ctx, obj_idx, "${value.name}");`
                  ])
                  .reduce<string[]>((arr, lines) => {
                    return [...arr, ...lines];
                  }, [])
              ];
            case "module":
              return [];
          }
        })(),
        "    duk_freeze(ctx, -1);",
        `    duk::put_prop_literal(ctx, rainbow, "${type.name}");`,
        "}"
      ].join("\n");
    }),
    "",
    "namespace rainbow::duk",
    "{",
    "    void register_all_modules(duk_context* ctx, duk_idx_t obj_idx)",
    "    {",
    ...typeInfo
      .filter(type => type.type !== "module")
      .map(
        type =>
          `        duk::register_module<${type.sourceName.replace(
            /rainbow::/g,
            ""
          )}>(ctx, obj_idx);`
      ),
    "    }",
    "}",
    "",
    "#endif",
    ""
  ].join("\n");
}

function generateTypeScriptDeclaration(typeInfo: TypeInfo[]): string {
  const getParams = (parameters: ParameterInfo[]) => {
    return parameters
      .map(p => `${p.name}: ${mapToTypeScriptType(p.type)}`)
      .join(", ");
  };
  const getFunctionDeclaration = (
    func: FunctionInfo,
    isClassMember: boolean,
    indent: string
  ) => {
    const functionKeyword = isClassMember ? "" : "function ";
    const name = toCamelCase(func.name);
    const params = getParams(func.parameters);
    const returnType = mapToTypeScriptType(func.returnType);
    return `${indent}    ${functionKeyword}${name}(${params}): ${returnType};`;
  };
  const getDeclaration = (module: TypeInfo, indent: string = ""): string => {
    switch (module.type) {
      case "class":
        return [
          `${indent}  export class ${module.name} {`,
          `${indent}    private readonly $type: "Rainbow.${module.name}";`,
          ...(!module.ctor
            ? []
            : [`${indent}    constructor(${getParams(module.ctor)});`]),
          ...module.methods.map(method =>
            getFunctionDeclaration(method, true, indent)
          ),
          `${indent}  }`
        ].join("\n");
      case "enum":
        return [
          `${indent}  export enum ${module.name} {`,
          ...module.values.map(
            value => `${indent}    ${value.name} = ${value.integralValue},`
          ),
          `${indent}  }`
        ].join("\n");
      case "module":
        return [
          `${indent}  export namespace ${module.name} {`,
          ...(!module.properties
            ? []
            : module.properties.map(
                prop => `${indent}    const ${prop.name}: ${prop.type}`
              )),
          ...(!module.functions
            ? []
            : module.functions.map(f =>
                getFunctionDeclaration(f, false, indent)
              )),
          ...(!module.types
            ? []
            : module.types.map(t => getDeclaration(t, "  "))),
          `${indent}  }`
        ].join("\n");
    }
  };
  return [
    "// This file was generated by `tools/generate-bindings.ts`. DO NOT MODIFY!",
    "//",
    "// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen",
    "// Distributed under the MIT License.",
    "// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)",
    "",
    "declare namespace Duktape {",
    "  const version: number;",
    "  const env: string;",
    "",
    "  function fin(o: {}, finalizer?: () => void): (() => void) | undefined;",
    '  function enc(format: "base64" | "hex" | "jc" | "jx", ...values: any[]): string;',
    '  function dec(format: "base64" | "hex" | "jc" | "jx", value: string): Buffer | string;',
    "  function info(value: any): {};",
    "  function act(index: number): {};",
    "  function compact(o: {}): void;",
    "",
    "  let errCreate: ((e: Error) => Error) | undefined;",
    "  let errThrow: ((e: Error) => Error) | undefined;",
    "",
    "  class Thread {",
    "    static current(): Thread;",
    "    static resume(thread: Thread, value?: any, thrown?: boolean): any;",
    "    static yield(value?: any, thrown?: boolean): any;",
    "",
    '    private readonly $type: "Duktape.Thread";',
    "",
    "    constructor(yielder: (value?: any) => any);",
    "  }",
    "}",
    "",
    "declare namespace Rainbow {",
    ((): string[] => typeInfo.map(m => getDeclaration(m)))().join("\n\n"),
    "}",
    ""
  ].join("\n");
}

function mapToTypeScriptType(type?: NativeType): string {
  return !type
    ? mapToTypeScriptTypeImpl(type)
    : type
        .split("|")
        .map(t => mapToTypeScriptTypeImpl(t.trim() as NativeType))
        .sort()
        .join(" | ");
}

function mapToTypeScriptTypeImpl(type?: NativeType): string {
  switch (type) {
    case "Animation::Callback":
      return "(animation: Animation, event: AnimationEvent) => void";
    case "Animation::Frames":
      return "number[]";
    case "Color":
      return "{ r: number, g: number, b: number, a: number }";
    case "SharedPtr<TextureAtlas>":
      return "Texture";
    case "SpriteRef":
      return "Sprite";
    case "Vec2f":
      return "{ x: number, y: number }";
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
      return type || "void";
  }
}

function preprocess(info: TypeInfo): TypeInfo {
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

function toCamelCase(str: string): string {
  let shouldUpperCase = false;
  return str
    .split("")
    .reduce<string[]>((arr, char) => {
      if (char === "_") {
        shouldUpperCase = true;
        return arr;
      }

      if (shouldUpperCase) {
        shouldUpperCase = false;
        arr.push(char.toUpperCase());
      } else {
        arr.push(char);
      }
      return arr;
    }, [])
    .join("");
}

function writeFile(path: string, data: string): void {
  fs.writeFile(path, data, { encoding: "utf8" }, err => {
    if (err) {
      throw err;
    }
    console.log(`-> ${path}`);
  });
}

const typeInfo = modules.map(preprocess);

const cppBindings = generateCppBindings(typeInfo);
writeFile(outputBindingsPath, cppBindings);

const tsDecl = generateTypeScriptDeclaration(typeInfo);
writeFile(outputDeclarationPath, tsDecl);
