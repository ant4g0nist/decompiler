cmake_minimum_required(VERSION 3.11)

project(decompiler CXX C)
set(RELEASE_VERSION "0.1-lldb")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Set the default build type to 'Release'
if(NOT CMAKE_BUILD_TYPE)
	set(default_build_type "Release")
	message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
	set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")

# Global defines.
add_definitions(-DRELEASE_VERSION="${RELEASE_VERSION}")
add_definitions(-D__MAC__)
add_definitions(-D__ARM64__)

# Subdirectories.
add_subdirectory(deps)
add_subdirectory(src)