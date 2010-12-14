#!/bin/sh
find . -name "*.mk" -o -name "Jamfile" -o -name "README" | xargs rm
rm -r src/{autofit,bdf,cache,cid,gxvalid,gzip,lzw,otvalid,pcf,pfr,psaux,tools,type1,type42,winfonts}
rm src/base/{ftbdf.c,ftcid.c,ftfstype.c,ftgasp.c,ftgxval.c,ftlcdfil.c,ftmac.c,ftmm.c,ftotval.c,ftpatent.c,ftpfr.c,fttype1.c,ftwinfnt.c,ftxf86.c}

echo "Disable the following files:

 - base/ftadvanc.c
 - base/ftcalc.c
 - base/ftgloadr.c
 - base/ftnames.c
 - base/ftobjs.c
 - base/ftoutln.c
 - base/ftrfork.c
 - base/ftstream.c
 - base/fttrigon.c
 - base/ftutil.c

Also disable any files in cff/, pshinter/, psmodule/,raster/, sfnt/, smooth/, truetype/ except the ones that have the same name.c.

Edit include/freetype/config/ftmodule.h and comment the following modules:

 - autofit_module_class
 - t1_driver
 - t1cid_driver_class
 - pfr_driver_class
 - t42_driver_class
 - winfnt_driver_class
 - pcf_driver_class
 - psaux_module_class
 - bdf_driver_class

Add the following C flags: -DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON"
