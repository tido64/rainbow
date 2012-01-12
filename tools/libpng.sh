#!/bin/sh
if [ ! -d lib/libpng ]; then
	echo "Could not find 'lib/libpng'"
	exit
fi
cd lib/libpng
rm -fr \
	*-sh \
	*.jpg \
	*.png \
	*.sh \
	aclocal.m4 \
	ANNOUNCE \
	CHANGES \
	CMakeLists.txt \
	configure \
	configure.ac \
	contrib \
	depcomp \
	example.c \
	INSTALL \
	libpng.3 \
	libpng-config.in \
	libpng.pc.in \
	libpngpf.3 \
	Makefile.* \
	missing \
	pngtest.c \
	projects \
	TODO
