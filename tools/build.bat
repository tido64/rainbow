@ECHO OFF
SET MSVC_VER="Visual Studio 12"
SET SOURCE_DIR=%~dp0\..

DEL /S /Q *.vcxproj *.vcxproj.filters *.vcxproj.user CMakeCache.txt cmake_install.cmake Rainbow.sdf Rainbow.sln 1> NUL 2>&1
RD /S /Q box2d.dir CMakeFiles Debug freetype.dir GLee.dir ipch lib lua.dir png.dir rainbow.dir Release Win32 1> NUL 2>&1
cmake %* -G %MSVC_VER% %SOURCE_DIR%
MSBuild Rainbow.sln /property:Configuration=Debug /maxcpucount
