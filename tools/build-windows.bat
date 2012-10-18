@ECHO OFF
SET MSVC_VER="Visual Studio 11"
SET SOURCE_DIR=%~dp0\..

DEL /S /Q *.vcxproj *.vcxproj.filters *.vcxproj.user CMakeCache.txt cmake_install.cmake Rainbow.sdf Rainbow.sln 1> NUL 2>&1
RD /S /Q box2d.dir CMakeFiles Debug freetype.dir GLee.dir ipch lua.dir png.dir rainbow.dir Win32 1> NUL 2>&1
cmake -DCANVAS=ON -G %MSVC_VER% %SOURCE_DIR%
MSBuild Rainbow.sln /p:Configuration=Debug
