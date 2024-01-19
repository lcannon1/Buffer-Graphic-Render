# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

#.rst:
# FindGLEW
# --------
#
# Find the OpenGL Extension Wrangler Library (GLEW)
#
# IMPORTED Targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the :prop_tgt:`IMPORTED` target ``GLEW::GLEW``,
# if GLEW has been found.
#
# Result Variables
# ^^^^^^^^^^^^^^^^
#
# This module defines the following variables:
#
# ::
#
#   GLEW_INCLUDE_DIRS - include directories for GLEW
#   GLEW_LIBRARIES - libraries to link against GLEW
#   GLEW_FOUND - true if GLEW has been found and can be used

# find the include directory if we don't know it already
if (NOT GLEW_INCLUDE_DIR)
	find_path( GLEW_INCLUDE_DIR 
		NAMES
			GL/glew.h
		HINTS
			"${GLEWDIR}/include"
			"$ENV{GLEWDIR}/include"
		PATHS
			"$ENV{PROGRAMFILES}/GLEW/include"
			"${OPENGL_INCLUDE_DIR}"
			/usr/openwin/share/include
			/usr/openwin/include
			/usr/X11R6/include
			/usr/include/X11
			/opt/graphics/OpenGL/include
			/opt/graphics/OpenGL/contrib/libglfw
			/usr/local/include
			/usr/include/GL
			/usr/include
		DOC 
			"The directory where GL/glew.h resides"
	)
endif()

# libraries
if (NOT GLEW_LIBRARY)
	find_library( GLEW_LIBRARY
		NAMES
			glew32
			glew32s
			glew
            GLEW
		HINTS
			"${GLEWDIR}/lib"
			"${GLEWDIR}/lib/x64"
			"${GLEWDIR}/lib/Release"
			"${GLEWDIR}/lib/Release/x64"
			"$ENV{GLEWDIR}/lib"
			"$ENV{GLEWDIR}/lib/x64"
			"$ENV{GLEWDIR}/lib/Release"
			"$ENV{GLEWDIR}/lib/Release/x64"
		PATHS
			"$ENV{PROGRAMFILES}/GLEW/lib"
			"$ENV{PROGRAMFILES}/GLEW/lib/Release"
			"$ENV{PROGRAMFILES}/GLEW/lib/Release/x64"
			"${OPENGL_LIBRARY_DIR}"
			/usr/lib64
			/usr/lib
			/usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
			/usr/local/lib64
			/usr/local/lib
			/usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
		DOC 
			"The GLEW library"
	)
endif()

if (WIN32 AND (NOT GLEW_BINARY))
	find_file( GLEW_BINARY
		NAMES
			glew32.dll
			glew.dll
		HINTS
			"${GLEWDIR}/bin"
			"${GLEWDIR}/bin/x64"
			"${GLEWDIR}/bin/Release"
			"${GLEWDIR}/bin/Release/x64"
			"$ENV{GLEWDIR}/bin"
			"$ENV{GLEWDIR}/bin/x64"
			"$ENV{GLEWDIR}/bin/Release"
			"$ENV{GLEWDIR}/bin/Release/x64"
		PATHS
			"$ENV{PROGRAMFILES}/GLEW/bin"
			"$ENV{PROGRAMFILES}/GLEW/bin/Release"
			"$ENV{PROGRAMFILES}/GLEW/bin/Release/x64"
		DOC 
			"The GLEW library dll"
	)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GLEW
    REQUIRED_VARS GLEW_INCLUDE_DIR GLEW_LIBRARY)

if(GLEW_FOUND)
	set(GLEW_INCLUDE_DIRS ${GLEW_INCLUDE_DIR})

	if(NOT GLEW_LIBRARIES)
		set(GLEW_LIBRARIES ${GLEW_LIBRARY})
	endif()
	
	if (NOT GLEW_BINARIES)
		set(GLEW_BINARIES ${GLEW_BINARY})
	endif()
endif()

mark_as_advanced(GLEW_INCLUDE_DIR GLEW_LIBRARY GLEW_BINARY)
