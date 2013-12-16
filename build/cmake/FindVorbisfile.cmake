# - Find the native Vorbisfile includes and library
#
# This module defines
# 	VORBISFILE_INCLUDE_DIR, where to find vorbisfile.h, etc.
# 	VORBISFILE_LIBRARIES, the libraries to link against to use Vorbisfile.

find_path(VORBISFILE_INCLUDE_DIR "vorbis/vorbisfile.h"
	/usr/include                     # Linux
	/usr/local/include               # Mac OS X (Homebrew)
	/opt/local/usr/include           # Mac OS X (MacPorts)
	/usr/i486-mingw32/include        # MinGW
	/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
	build/windows/include            # Windows
)
find_library(VORBISFILE_LIBRARIES NAMES vorbisfile libvorbisfile libvorbisfile_static
                                  HINTS build/windows/lib)

if(MINGW OR WIN32)
	find_path(VORBIS_INCLUDE_DIR "vorbis/codec.h"
		/usr/include                     # Linux
		/usr/i486-mingw32/include        # MinGW
		/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
		build/windows/include            # Windows
	)
	find_library(VORBIS_LIBRARY NAMES vorbis libvorbis libvorbis_static
	                            HINTS build/windows/lib)

	find_path(OGG_INCLUDE_DIR "ogg/ogg.h"
		/usr/include                     # Linux
		/usr/i486-mingw32/include        # MinGW
		/usr/x86_64-w64-mingw32/include  # MinGW-w64 x64
		build/windows/include            # Windows
	)
	find_library(OGG_LIBRARY NAMES ogg libogg libogg_static
	                         HINTS build/windows/lib)

	set(VORBISFILE_INCLUDE_DIR ${VORBISFILE_INCLUDE_DIR} ${VORBIS_INCLUDE_DIR} ${OGG_INCLUDE_DIR})
	set(VORBISFILE_LIBRARIES ${VORBISFILE_LIBRARIES} ${VORBIS_LIBRARY} ${OGG_LIBRARY})

	# Force use of found libraries (because sometimes it just won't obey)
	foreach(lib ${VORBISFILE_LIBRARIES})
		get_filename_component(libname ${lib} NAME_WE)
		add_library(${libname} STATIC IMPORTED)
		set_target_properties(${libname} PROPERTIES IMPORTED_LOCATION ${lib})
		list(APPEND VORBIS_LIBRARIES ${libname})
	endforeach()
	set(VORBISFILE_LIBRARIES ${VORBIS_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbisfile DEFAULT_MSG VORBISFILE_LIBRARIES VORBISFILE_INCLUDE_DIR)

mark_as_advanced(VORBISFILE_LIBRARIES VORBISFILE_INCLUDE_DIR)
