# - Find the native Vorbisfile includes and library
#
# This module defines
# 	VORBISFILE_INCLUDE_DIR, where to find vorbisfile.h, etc.
# 	VORBISFILE_LIBRARIES, the libraries to link against to use Vorbisfile.
# 	VORBISFILE_DEFINITIONS - You should add_definitons(${VORBISFILE_DEFINITIONS}) before compiling code that includes Vorbisfile library files.
# 	VORBISFILE_FOUND, If false, do not try to use Vorbisfile.
# also defined, but not for general use are
# 	VORBISFILE_LIBRARY, where to find the Vorbisfile library.

find_path(VORBISFILE_INCLUDE_DIR "vorbis/vorbisfile.h"
	/usr/include                     # Linux
	/opt/local/usr/include           # Mac OS X (MacPorts)
	/usr/i486-mingw32/include        # MinGW
	/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
	../libs/include                  # Windows
)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile libvorbisfile
                                HINTS ../libs/Win32)

if(MINGW)
	find_path(VORBIS_INCLUDE_DIR "vorbis/codec.h"
		/usr/include                     # Linux
		/usr/i486-mingw32/include        # MinGW
		/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
		../libs/include                  # Windows
	)
	find_library(VORBIS_LIBRARY NAMES vorbis libvorbis
	                            HINTS ../libs/Win32)

	find_path(OGG_INCLUDE_DIR "ogg/ogg.h"
		/usr/include                     # Linux
		/usr/i486-mingw32/include        # MinGW
		/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
		../libs/include                  # Windows
	)
	find_library(OGG_LIBRARY NAMES ogg libogg
	                         HINTS ../libs/Win32)

	set(VORBISFILE_INCLUDE_DIR ${VORBISFILE_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR} ${OGG_INCLUDE_DIR})
	set(VORBISFILE_LIBRARY ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY} ${OGG_LIBRARY})

	# Force use of found libraries (because sometimes it just won't obey)
	foreach(lib ${VORBISFILE_LIBRARY})
		get_filename_component(libname ${lib} NAME_WE)
		add_library(${libname} STATIC IMPORTED)
		set_target_properties(${libname} PROPERTIES IMPORTED_LOCATION ${lib})
		list(APPEND VORBIS_LIBRARIES ${libname})
	endforeach()
	set(VORBISFILE_LIBRARY ${VORBIS_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbisfile DEFAULT_MSG VORBISFILE_LIBRARY VORBISFILE_INCLUDE_DIR)

mark_as_advanced(VORBISFILE_LIBRARY VORBISFILE_INCLUDE_DIR)
