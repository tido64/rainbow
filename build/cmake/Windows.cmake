if(WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".dll.a" ".dll")
  list(APPEND RESOURCE_FILES
    .editorconfig
    build/cmake/Audio.cmake
    build/cmake/Box2D.cmake
    build/cmake/Duktape.cmake
    build/cmake/FreeType.cmake
    build/cmake/libpng.cmake
    build/cmake/PhysicsFS.cmake
    build/cmake/SDL2.cmake
    build/cmake/Utilities.cmake
    build/cmake/Windows.cmake
    build/cmake/zlib.cmake
    src/Resources/Rainbow.natvis
    src/Resources/Rainbow.rc
  )
  add_definitions(
    -D_CRT_SECURE_NO_WARNINGS=1
    -D_SCL_SECURE_NO_WARNINGS=1
    -DNO_STATE_FLAGS=1
    -DNOATOM=1             # Atom Manager routines
    -DNOCOLOR=1            # Screen colors
    -DNOCOMM=1             # COMM driver routines
    -DNODEFERWINDOWPOS=1   # DeferWindowPos routines
    -DNODESKTOP=1          # Desktop-specific access flags
    -DNODRAWTEXT=1         # DrawText() and DT_*
    -DNOGDICAPMASKS=1      # CC_*, LC_*, PC_*, CP_*, TC_*, RC_
    -DNOHELP=1             # Help engine interface.
    -DNOICONS=1            # IDI_*
    -DNOKANJI=1            # Kanji support stuff.
    -DNOKERNEL=1           # All KERNEL defines and routines
    -DNOKEYSTATES=1        # MK_*
    -DNOLANGUAGE=1         # Language dependent Routines
    -DNOMB=1               # MB_* and MessageBox()
    -DNOMCX=1              # Modem Configuration Extensions
    -DNOMDI=1
    -DNOMEMMGR=1           # GMEM_*, LMEM_*, GHND, LHND, associated routines
    -DNOMENUS=1            # MF_*
    -DNOMETAFILE=1         # typedef METAFILEPICT
    -DNOMINMAX=1           # Macros min(a,b) and max(a,b)
    -DNOOPENFILE=1         # OpenFile(), OemToAnsi, AnsiToOem, and OF_*
    -DNOPROFILER=1         # Profiler interface.
    -DNORASTEROPS=1        # Binary and Tertiary raster ops
    -DNORESOURCE=1         # Predefined resource types
    -DNOSCROLL=1           # SB_* and scrolling routines
    -DNOSECURITY=1
    -DNOSERVICE=1          # All Service Controller routines, SERVICE_ equates, etc.
    -DNOSHOWWINDOW=1       # SW_*
    -DNOSOUND=1            # Sound driver routines
    -DNOSYSCOMMANDS=1      # SC_*
    -DNOSYSPARAMSINFO      # Parameter for SystemParametersInfo
    -DNOTEXTMETRIC=1       # typedef TEXTMETRIC and associated routines
    -DNOVIRTUALKEYCODES=1  # VK_*
    -DNOWH=1               # SetWindowsHook and WH_*
    -DNOWINDOWSTATION=1
    -DNOWINOFFSETS=1       # GWL_*, GCL_*, associated routines
    -DVC_EXTRALEAN=1
    -DWIN32_LEAN_AND_MEAN=1
  )
  if(MSVC)
    # Warning C4065: switch statement contains 'default' but no 'case' labels
    # Warning C4244: 'argument': conversion from 'type1' to 'type2', possible loss of data
    # Warning C4456: declaration of 'x' hides previous local declaration
    # Warning C4800: 'type': forcing value to bool 'true' or 'false' (performance warning)
    # /GR-: Disables run-time type information (RTTI).
    # /Oi: Generates intrinsic functions.
    # /W0: Disables all compiler warnings.
    set(RAINBOW_CXX_WARNINGS /wd4065 /wd4244 /wd4456 /wd4800 /W4 /WX)
    set(CFLAG_NO_WARNINGS /W0)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAG_NO_WARNINGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /Oi")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /std:c++17 /permissive-")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Oi")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi")
    string(REGEX REPLACE "/D NDEBUG" "" MSVC_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${MSVC_CXX_FLAGS_RELWITHDEBINFO}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:libcmtd.lib")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /NODEFAULTLIB:msvcrt.lib")

    set(RAINBOW_VCXPROJ_USER "${CMAKE_CURRENT_BINARY_DIR}/Rainbow.vcxproj.user")
    if(NOT EXISTS ${RAINBOW_VCXPROJ_USER})
      file(WRITE ${RAINBOW_VCXPROJ_USER}
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
        "<Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
        "  <PropertyGroup>\n"
        "    <LocalDebuggerCommandArguments>${PROJECT_SOURCE_DIR}/js</LocalDebuggerCommandArguments>\n"
        "    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n"
        "  </PropertyGroup>\n"
        "</Project>\n"
      )
    endif()
  else()
    add_definitions(
      -D__MSVCRT__
      -D__MSVCRT_VERSION__=0x800
      -D_WIN32_WINNT=0x0600
      -DNTDDI_VERSION=0x06000000
      -DWINVER=0x0600
    )
  endif()
endif()
