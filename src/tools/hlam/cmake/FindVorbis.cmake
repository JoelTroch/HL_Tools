# MSVC built vorbis may be named vorbis_static
# The provided project files name the library with the lib prefix.
find_library(VORBIS_LIBRARY NAMES vorbis vorbis_static libvorbis libvorbis_static)
find_library(VORBISFILE_LIBRARY NAMES vorbisfile vorbisfile_static libvorbisfile libvorbisfile_static)

# Handle the QUIETLY and REQUIRED arguments and set VORBIS_FOUND
# to TRUE if all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VORBIS DEFAULT_MSG VORBIS_LIBRARY VORBISFILE_LIBRARY)

if(VORBIS_FOUND)
	set(VORBIS_LIBRARIES ${VORBISFILE_LIBRARY} ${VORBIS_LIBRARY})
else(VORBIS_FOUND)
	set(VORBIS_LIBRARIES)
endif(VORBIS_FOUND)

mark_as_advanced(VORBIS_LIBRARY VORBISFILE_LIBRARY)
