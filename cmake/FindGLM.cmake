# Try to find GLFW library and include path.
# Once done this will define
#
# GLFW_FOUND
# GLFW_INCLUDE_DIR
# GLFW_LIBRARIES
#

find_path( GLM_INCLUDE_DIR 
    NAMES
        glm/glm.hpp
    HINTS
        "${GLM_DIR}"
        "$ENV{GLM_DIR}"
    PATHS
        "$ENV{PROGRAMFILES}/GLM/include"
        "$ENV{PROGRAMFILES}/GLM"
        "${OPENGL_INCLUDE_DIR}"
        /usr/local/include
        /usr/include/GL
        /usr/include
    DOC 
        "The directory where glm/glm.hpp resides"
)

set( GLM_FOUND "NO" )

if(GLFW_INCLUDE_DIR)
	set(GLM_FOUND "YES")
endif(GLFW_INCLUDE_DIR)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(GLM 
    REQUIRED_VARS
        GLM_INCLUDE_DIR
)

mark_as_advanced(
  GLM_INCLUDE_DIR
)


