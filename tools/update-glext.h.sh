#!/bin/bash
DST="lib/GL/glext.h"
URL="http://www.opengl.org/registry/api/glext.h"
[[ ! -f $DST ]] && echo "Please run from root of repository" && exit 1
curl $URL -o $DST
