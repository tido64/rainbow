#!/bin/sh
dst=$(cd -P "$(dirname $0)/../src/ThirdParty/libpng" && pwd)
[ -d "$dst" ] || exit $(stat "$dst")
[ -f png.c ] || exit $(stat png.c)
./autogen.sh
cp "$dst/pngusr.h" .
make clean > /dev/null
CPPFLAGS='-DPNG_USER_CONFIG' ./configure --disable-dependency-tracking --enable-shared=no
make pnglibconf.h
cp config.h "$dst"
cp pnglibconf.h "$dst"
