#!/usr/bin/python
# Copyright (c) 2010-16 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

import os
import re
import sys

bytes_per_line = 0  # 12

def main(resource):
    output = os.path.split(resource)[1]
    name = re.sub('[^\w]', '_', output)
    guard = name.upper() + "_H\n"
    rez = '#ifndef ' + guard \
        + '#define ' + guard + "\n" \
        + 'constexpr unsigned char ' + name + "[]{"

    f = open(resource, 'rb')
    if bytes_per_line > 0:
        rez += "\n"
        bytes = f.read(bytes_per_line)
        while bytes:
            rez += '    ' + ' '.join(['0x%02x,' % b for b in bytes]) + "\n"
            bytes = f.read(bytes_per_line)
        rez = rez[:-2] + '};'
    else:
        bytes = f.read()
        rez += ''.join(['%u,' % b for b in bytes])[:-1] + '};'
    f.close()

    f = open(output + '.h', 'w')
    f.write(rez + "\n\n#endif  // " + guard)
    f.close()
    return 0

if __name__ == "__main__":
    sys.exit(main(sys.argv[1]))
