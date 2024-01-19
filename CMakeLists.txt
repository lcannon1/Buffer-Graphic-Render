cmake_minimum_required(VERSION 2.8...3.19)
project(GLapp)
set(TARGET GLapp)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${TARGET})

# set C++ version to use
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# I'm lazy here, and building with all cpp files in src
file(GLOB SOURCES  "src/*.cpp")
file(GLOB INCLUDES "src/*.hpp")
file(GLOB INLINES  "src/*.inl")
add_executable(${TARGET} ${SOURCES} ${INCLUDES} ${INLINES})

# set up config.h to find data directory
set(PROJECT_BASE_DIR "${PROJECT_SOURCE_DIR}")
set(PROJECT_DATA_DIR "${PROJECT_BASE_DIR}/data")
configure_file(src/config.h.in config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# add GLFW, GLEW, GLM, and OpenGL
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(GLFW REQUIRED)
include_directories(${GLFW_INCLUDE_DIR})
target_link_libraries(GLapp ${GLFW_LIBRARIES})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
target_link_libraries(GLapp ${GLEW_LIBRARIES})
if (WIN32)
	add_custom_command(TARGET GLapp POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${GLEW_BINARIES}" $<TARGET_FILE_DIR:GLapp>)
endif()

find_package(GLM REQUIRED)
include_directories(${GLM_INCLUDE_DIR})

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
target_link_libraries(GLapp ${OPENGL_LIBRARIES})

# other libraries
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(CMAKE_EXE_LINKER_FLAGS "-lXrandr -lXinerama -lXcursor -lXi")
endif()
