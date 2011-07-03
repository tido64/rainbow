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
	/usr/include  # Linux
)
set(VORBISFILE_NAMES ${VORBISFILE_NAMES} vorbisfile libvorbisfile)
find_library(VORBISFILE_LIBRARY NAMES ${VORBISFILE_NAMES})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vorbisfile DEFAULT_MSG VORBISFILE_LIBRARY VORBISFILE_INCLUDE_DIR)

mark_as_advanced(VORBISFILE_LIBRARY VORBISFILE_INCLUDE_DIR)
