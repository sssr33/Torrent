cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(BuildApp VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(BuildAppConfig.h.in BuildAppConfig.h)

file(GLOB_RECURSE MyCppSources "./*.cpp" "./*.cxx" "./*.c" "./*.h" "./*.hpp")

# https://stackoverflow.com/a/36667361
# disable ZERO_CHECK
# set(CMAKE_SUPPRESS_REGENERATION true)

add_executable(BuildApp ${MyCppSources})

# add the binary tree to the search path for include files
# so that we will find BuildAppConfig.h.h
target_include_directories(BuildApp PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${MyCppSources})

# https://cmake.org/cmake/help/latest/prop_dir/VS_STARTUP_PROJECT.html
# https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT BuildApp)

if(MSVC)
 target_compile_options(BuildApp PRIVATE "/MP")
endif()
