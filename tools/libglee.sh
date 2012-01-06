#!/bin/bash
if [ ! -d lib/GL ]; then
	echo "Could not find 'lib/GL'";
	exit
fi
git clone git://glee.git.sourceforge.net/gitroot/glee/glee /tmp/glee
mv /tmp/glee/DATA/output/GLee.c lib/GL/
mv /tmp/glee/include/GL/GLee.h lib/GL/
rm -fr /tmp/glee
patch -Nr - lib/GL/GLee.h < tools/patches/glee.patch
