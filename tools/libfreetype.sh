#!/bin/sh
find . -name "*.mk" -o -name "Jamfile" -o -name "README" | xargs rm
rm -r src/{autofit,bdf,cache,cid,gxvalid,gzip,lzw,otvalid,pcf,pfr,psaux,tools,type1,type42,winfonts}
rm src/base/{ftbdf.c,ftcid.c,ftfstype.c,ftgasp.c,ftgxval.c,ftlcdfil.c,ftmac.c,ftmm.c,ftotval.c,ftpatent.c,ftpfr.c,fttype1.c,ftwinfnt.c,ftxf86.c}

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
echo "Also disable any files in cff/, pshinter/, psnames/, raster/, sfnt/, smooth/, truetype/ except the ones that match <dirname>.c"
echo
echo "Edit include/freetype/config/ftmodule.h and comment the following modules:"
echo
echo " - autofit_module_class"
echo " - t1_driver"
echo " - t1cid_driver_class"
echo " - pfr_driver_class"
echo " - t42_driver_class"
echo " - winfnt_driver_class"
echo " - pcf_driver_class"
echo " - psaux_module_class"
echo " - bdf_driver_class"
echo
echo "Add the following C flags: -DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON"
