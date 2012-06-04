#!/bin/sh
if [ ! -d lib/libpng ]; then
	echo "Could not find 'lib/libpng'"
	exit
fi
cd lib/libpng
rm -fr \
	*-sh \
	*.dfa \
	*.jpg \
	*.png \
	*.sh \
	aclocal.m4 \
	ANNOUNCE \
	CHANGES \
	CMakeLists.txt \
	config.guess \
	config.h.in \
	config.sub \
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
	png.5 \
	pngtest.c \
	pngwrite.c \
	projects \
	TODO
