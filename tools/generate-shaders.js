// Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
// Distributed under the MIT License.
// (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

// @ts-check
"use strict";

const fs = require("fs");
const path = require("path");
const {
  makeBanner,
  makeSafeName,
  writeFile: writeFileInternal
} = require("./import-asset");

const EOL = "\n";

const shadersPath = path.resolve(__dirname, "..", "src", "Graphics", "Shaders");
const shaders = fs
  .readdirSync(shadersPath)
  .filter(isShader)
  .sort();

const copyright = makeBanner(__filename);

writeFile(
  "Shaders.cpp",
  `${copyright}

#include "Graphics/Shaders.h"

// clang-format off

namespace
{
${shaders.map(embedShader).join(`${EOL}${EOL}`)}
}  // namespace

${shaders.map(defineGetter).join(`${EOL}${EOL}`)}

// clang-format on
`
);

writeFile(
  "Shaders.h",
  `${copyright}

#include "Common/String.h"
#include "Graphics/ShaderDetails.h"

namespace rainbow::gl
{
${shaders.map(declareGetter).join(EOL)}
}  // namespace rainbow::gl
`
);

/**
 * Declares getter for specified shader.
 * @param {string} file
 * @returns {string}
 */
function declareGetter(file) {
  return isShaderHeader(file)
    ? `    auto ${makeSafeName(file)}() -> czstring;`
    : `    auto ${makeSafeName(file)}() -> Shader::Params;`;
}

/**
 * Defines getter for specified shader.
 * @param {string} file
 * @returns {string}
 */
function defineGetter(file) {
  const isHeader = isShaderHeader(file);
  const name = makeSafeName(file);
  const [returnType, returnValue] = isHeader
    ? ["rainbow::czstring", `k${name}`]
    : [
        "Shader::Params",
        `{${inferShaderType(file)}, 0, "Shaders/${file}", k${name}}`
      ];
  return `auto rainbow::gl::${name}() -> ${returnType}
{
    return ${returnValue};
}`;
}

/**
 * Embeds shader.
 * @param {string} file
 * @returns {string}
 */
function embedShader(file) {
  return `    constexpr char k${makeSafeName(file)}[] =
${fs
  .readFileSync(path.join(shadersPath, file))
  .toString()
  .split(EOL)
  .filter(isCodeLine)
  .map(trimLine)
  .join(EOL)};`;
}

/**
 * Infers shader type from filename.
 * @param {string} file
 * @returns {string}
 */
function inferShaderType(file) {
  switch (path.extname(file)) {
    case ".frag":
      return "Shader::kTypeFragment";
    case ".vert":
      return "Shader::kTypeVertex";
    default:
      throw new Error(`Unknown shader: ${file}`);
  }
}

/**
 * Returns whether the line is considered a code line.
 * @param {string} line
 * @returns {boolean}
 */
function isCodeLine(line) {
  const trimmedLine = line.trim();
  return Boolean(trimmedLine) && !trimmedLine.startsWith("//");
}

/**
 * Returns whether specified file is a shader.
 * @param {string} file
 * @returns {boolean}
 */
function isShader(file) {
  return [".glsl", ".frag", ".vert"].includes(path.extname(file));
}

/**
 * Returns whether specified file is a shader header.
 * @param {string} file
 * @returns {boolean}
 */
function isShaderHeader(file) {
  return path.extname(file) === ".glsl";
}

/**
 * Strips comments from specified line.
 * @param {string} line
 * @returns {string}
 */
function stripComments(line) {
  return line.replace(/\s+\/\/.*/g, "");
}

/**
 * Trims specified line.
 * @param {string} line
 * @returns {string}
 */
function trimLine(line) {
  const trimmedLine = line.trimLeft();
  const indent = " ".repeat(line.length - trimmedLine.length);
  return `        ${indent}"${stripComments(trimmedLine)}\\n"`;
}

/**
 * Asynchronously writes data to a file, replacing the file if needed.
 * @param {string} filename
 * @param {string} data
 */
function writeFile(filename, data) {
  writeFileInternal(path.join(shadersPath, "..", filename), data);
}
