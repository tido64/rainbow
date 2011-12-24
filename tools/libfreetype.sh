#!/bin/sh
if [ ! -f include/ft2build.h ]; then
	echo "Could not find 'include/ft2build.h'"
	exit
fi

TOOLS="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

rm -fr ChangeLog* Jam* Makefile README* autogen.sh builds configure devel docs modules.cfg objs version.sed vms_make.com \
src/{autofit,bdf,cache,cid,gxvalid,gzip,lzw,otvalid,pcf,pfr,psaux,tools,type1,type42,winfonts} \
src/base/{ftbdf.c,ftcid.c,ftfstype.c,ftgasp.c,ftgxval.c,ftlcdfil.c,ftmac.c,ftmm.c,ftotval.c,ftpatent.c,ftpfr.c,fttype1.c,ftwinfnt.c,ftxf86.c}
find . -name '*.mk' -exec rm {} \;
find . -name Jamfile -exec rm {} \;
patch -Nr - include/freetype/config/ftmodule.h < $TOOLS/patches/freetype.patch

echo "Disable the following files:"
echo
echo " - base/ftadvanc.c"
echo " - base/ftcalc.c"
echo " - base/ftgloadr.c"
echo " - base/ftobjs.c"
echo " - base/ftoutln.c"
echo " - base/ftrfork.c"
echo " - base/ftsnames.c"
echo " - base/ftstream.c"
echo " - base/fttrigon.c"
echo " - base/ftutil.c"
echo
echo "Also disable any files in cff/, pshinter/, psnames/, raster/, sfnt/, smooth/,"
echo "truetype/ that do not match <dirname>.c"
echo
echo "Finally, add the following C flags: -DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON"
