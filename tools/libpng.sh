#!/bin/sh
if [ ! -d lib/libpng ]; then
	echo "Could not find 'lib/libpng'"
	exit
fi
rm -fr contrib/gregbook contrib/pngminim contrib/pngminus contrib/pngsuite contrib/visupng/{PngFile.*,README.txt,VisualPng.*,resource.h} example.c pngtest.c
