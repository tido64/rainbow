#!/usr/bin/python
# Copyright (c) 2010-14 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

import os
import sys

bytes_per_line = 0

def main(resource):
	output = os.path.split(resource)[1]
	rez = 'const unsigned char ' + output.replace('.', '_') + "[] = {"

	f = open(resource, 'rb')
	if bytes_per_line > 0:
		rez += "\n"
		bytes = f.read(bytes_per_line)
		while bytes:
			rez += "\t" + ''.join(['0x%02x, ' % b for b in bytes]).strip() + "\n"
			bytes = f.read(bytes_per_line)
		rez = rez[:-2] + "\n};\n"
	else:
		bytes = f.read()
		rez += ' ' + ''.join(['%u,' % b for b in bytes])[:-1] + " };\n"
	f.close()

	f = open(output + '.h', 'w')
	f.write(rez)
	f.close()
	return 0

if __name__ == "__main__":
	sys.exit(main(sys.argv[1]))
