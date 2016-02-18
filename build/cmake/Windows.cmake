if(WIN32)
  set(CFLAG_NO_WARNINGS /W0)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".lib" ".dll.a" ".dll")
  list(APPEND RESOURCE_FILES
       .editorconfig
       build/cmake/Windows.cmake
       build/windows/Rainbow.natvis
       build/windows/Rainbow.rc
       src/DotNet/Rainbow/.editorconfig)
  add_definitions(
      -D_CRT_SECURE_NO_WARNINGS=1
      -D_SCL_SECURE_NO_WARNINGS=1
      -DNO_STATE_FLAGS=1
      -DNOATOM=1             # Atom Manager routines
      -DNOCOLOR=1            # Screen colors
      -DNOCOMM=1             # COMM driver routines
      -DNOCTLMGR=1           # Control and Dialog routines
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
      -DNOSYSMETRICS=1       # SM_*
      -DNOSYSPARAMSINFO      # Parameter for SystemParametersInfo
      -DNOTEXTMETRIC=1       # typedef TEXTMETRIC and associated routines
      -DNOVIRTUALKEYCODES=1  # VK_*
      -DNOWH=1               # SetWindowsHook and WH_*
      -DNOWINABLE=1
      -DNOWINDOWSTATION=1
      -DNOWINMESSAGES=1      # WM_*, EM_*, LB_*, CB_*
      -DNOWINOFFSETS=1       # GWL_*, GCL_*, associated routines
      -DNOWINSTYLES=1        # WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
      -DVC_EXTRALEAN=1
      -DWIN32_LEAN_AND_MEAN=1)
  if(MSVC)
    # Warning C4065: switch statement contains 'default' but no 'case' labels
    # Warning C4244: 'argument': conversion from 'type1' to 'type2', possible loss of data
    # Warning C4456: declaration of 'x' hides previous local declaration
    # Warning C4800: 'type': forcing value to bool 'true' or 'false' (performance warning)
    # /GR-: Disables run-time type information (RTTI).
    # /INCREMENTAL:NO: Disables incremental linking.
    # /MT: Creates a multithreaded executable file using LIBCMT.lib.
    # /MTd: Creates a debug multithreaded executable file using LIBCMTD.lib.
    # /Oi: Generates intrinsic functions.
    # /W0: Disables all compiler warnings.
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CFLAG_NO_WARNINGS}")
    set(CMAKE_C_FLAGS_MINSIZEREL "${CMAKE_C_FLAGS_MINSIZEREL} /Oi /MT")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi /MT")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "${CMAKE_C_FLAGS_RELWITHDEBINFO} /MTd")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR- /wd4065 /wd4244 /wd4456 /wd4800 /W4 /std:c++latest")
    set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /Oi /MT")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi /MT")
    string(REGEX REPLACE "/D NDEBUG" "" MSVC_CXX_FLAGS_RELWITHDEBINFO ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${MSVC_CXX_FLAGS_RELWITHDEBINFO} /MTd")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:msvcrt.lib")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /INCREMENTAL:NO /NODEFAULTLIB:libcmt.lib")
  else()
    add_definitions(
        -D__MSVCRT__
        -D__MSVCRT_VERSION__=0x800
        -D_WIN32_WINNT=0x0600
        -DNTDDI_VERSION=0x06000000
        -DWINVER=0x0600)
  endif()
endif()
