#!/usr/bin/python
# Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

from datetime import date
import os
import re
import sys

shaders_dir = os.path.join(sys.path[0], "..", "src", "Graphics", "Shaders")

RE_COMMENT = re.compile(r'\s*?//')
RE_INDENT = re.compile(r'^(\s+)')
RE_WHITESPACE = re.compile(r'^\s+$')

def main():
    shaders_h = []
    for dirname, dirnames, filenames in os.walk(shaders_dir):
        for filename in filenames:
            if not filename.endswith("fsh") and not filename.endswith("vsh"):
               continue
            level = 0
            shader = [ "const char k" + filename + "[] =" ]
            shader[0] = re.sub(r'\.([fv])sh', r'\1', shader[0])
            f = open(os.path.join(dirname, filename), 'r')
            for line in f:
                if RE_COMMENT.search(line):
                    continue
                elif RE_WHITESPACE.match(line):
                    if len(shader) == 1:
                        continue
                    shader.append("")
                else:
                    start_comment = line.find("//")
                    if start_comment >= 0:
                        line = line[:start_comment]
                    line = line.rstrip()
                    prefix = "\t"
                    m = RE_INDENT.search(line)
                    if m:
                        prefix += m.group(1)
                    shader.append(prefix + '"' + line.lstrip() + '\\n"')
            f.close()
            shaders_h.append("\n".join(shader) + ";\n")
    f = open(os.path.join(shaders_dir, "Shaders.h"), 'w')
    f.write("// This file was auto-generated with shaders-gen.py.\n")
    f.write("// Copyright (c) " + str(date.today().year) + " Bifrost Entertainment AS and Tommy Nguyen. All rights reserved.\n\n")
    f.write("\n".join(shaders_h))
    f.close()
    return 0

if __name__ == "__main__":
    main()
