#!/usr/bin/python
from datetime import date
import os
import re

shaders_dir = "src/Graphics/Shaders"

comment = re.compile(r'\s*?//')
whitespace = re.compile(r'^\s+$')

def main():
	shaders_h = []
	for dirname, dirnames, filenames in os.walk(shaders_dir):
		for filename in filenames:
			if not filename.endswith('fsh') and not filename.endswith('vsh'):
				continue
			level = 0
			shader = [ 'const char ' + filename.lower() + '[] =' ]
			shader[0] = shader[0].replace('.', '_')
			f = open(os.path.join(dirname, filename), 'r')
			for line in f:
				if comment.search(line):
					continue
				elif whitespace.match(line):
					if len(shader) == 1:
						continue
					shader.append('')
				else:
					start_comment = line.find('//')
					if start_comment >= 0:
						line = line[:start_comment]
					line = line.rstrip()
					shader.append('\t"' + line + '\\n"')
			f.close()
			shaders_h.append("\n".join(shader) + ";\n")
	f = open(os.path.join(shaders_dir, "Shaders.h"), 'w')
	f.write("// This file was auto-generated with shaders-gen.py.\n")
	f.write("// Copyright " + str(date.today().year) + " Bifrost Entertainment AS. All rights reserved.\n\n")
	f.write("\n".join(shaders_h))
	f.close()
	return 0

if __name__ == "__main__":
	main()
