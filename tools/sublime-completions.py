#!/usr/bin/env python
# Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

from datetime import date
import re
import os

class NumberedParameters(object):
    def __init__(self):
        self.count = 0
    def __call__(self, match):
        self.count += 1
        return '{}${{{}:{}}}{}'.format(match.group(1), self.count, match.group(2), match.group(3))

class SublimeCompletions(object):
    REGEX_INSTANCE = re.compile(r'&lt;(.*?)&gt;')
    REGEX_PARAMS = re.compile(r'([ \(])([\w "&\+\-\.;=]+)([\),])')
    REGEX_SYNTAX = re.compile(r'^### (\w.*?)[\n\[]')
    def format(self, line):
        return '    "{}",'.format(self.REGEX_INSTANCE.sub(r'<\1>', line.replace('"', '\\"')))
    def parse(self, ref):
        return filter((lambda line: line != None), map(self.parse_line, ref))
    def parse_line(self, line):
        match = self.REGEX_SYNTAX.match(line)
        if match:
            return self.format(self.REGEX_PARAMS.sub(NumberedParameters(), match.group(1)).strip())
    def template(self):
        return '{{\n  "scope": "source.lua",\n  "completions": [\n{}\n  ]\n}}\n'

def generate(g, source):
    out = '\n'.join(g.parse(source))
    return g.template().format(out[:len(out) - 1])

if __name__ == '__main__':
    f = open(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'doc', 'programming', 'lua', 'api.md'), 'r')
    if f:
        s = generate(SublimeCompletions(), f)
        f.close()
        f = open('Rainbow.sublime-completions', 'w')
        f.write('// This file was generated with {}.\n'.format(os.path.basename(__file__)))
        f.write('// Copyright (c) {} Bifrost Entertainment AS and Tommy Nguyen.\n'.format(str(date.today().year)))
        f.write('// Distributed under the MIT License.\n')
        f.write(s)
        f.close()
