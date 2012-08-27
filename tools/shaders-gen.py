#!/usr/bin/python
from datetime import date
import os

shaders_dir = "src/Graphics/Shaders"

def main():
	shaders_h = []
	for dirname, dirnames, filenames in os.walk(shaders_dir):
		for filename in filenames:
			if not filename.endswith('fsh') and not filename.endswith('vsh'):
				continue
			level = 0
			shader = [ 'const char ' + filename.lower() + '[] =' ]
			shader[0] = shader[0].replace('.', '_')
			skip = [ False ]
			f = open(os.path.join(dirname, filename), 'r')
			for line in f:
				if line.startswith('#'):
					if line.find('endif') > 0:
						level = max(0, level - 1)
					elif line.find('else') > 0:
						skip[level] = not skip[level]
					elif line.find('if') > 0:
						level += 1
						if len(skip) == level:
							skip.append(False)
						skip[level] = line.find('ifdef GL_ES') < 0 and line.find('if GL_FRAGMENT_PRECISION_HIGH') < 0
					#print line, skip, level
					continue
				elif skip[level] or line.startswith('//'):
					continue
				elif line == "\n":
					if len(shader) == 1:
						continue
					shader.append('')
				else:
					comment = line.find('//')
					if comment >= 0:
						line = line[:comment]
					line = line.rstrip()
					shader.append('"' + line + '"')
					skip[level] = False
			f.close()
			shaders_h.append("\n".join(shader) + ";\n")
	f = open(os.path.join(shaders_dir, "Shaders.h"), 'w')
	f.write("// This file was auto-generated with shaders-gen.py.\n")
	f.write("// Copyright " + str(date.today().year) + " Bifrost Entertainment AS. All rights reserved.\n\n")
	f.write("#ifdef RAINBOW_IOS\n")
	f.write("#	define GL_DRAW_FRAMEBUFFER GL_DRAW_FRAMEBUFFER_APPLE\n")
	f.write("#endif\n\n")
	f.write("\n".join(shaders_h))
	f.close()
	return 0

if __name__ == "__main__":
	main()
