#!/bin/sh
# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

dst=$(cd -P "$(dirname $0)/../src/ThirdParty/libpng" && pwd)
[ -d "$dst" ] || exit $(stat "$dst")
[ -f png.c ] || exit $(stat png.c)
./autogen.sh || exit 1
cp "$dst/pngusr.h" .
CPPFLAGS='-DPNG_USER_CONFIG' ./configure --disable-dependency-tracking --enable-shared=no || exit 1
make pnglibconf.h || exit 1
cp config.h "$dst"
cp pnglibconf.h "$dst"
