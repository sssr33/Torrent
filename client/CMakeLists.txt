cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Client VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited configurations" FORCE)

file(GLOB_RECURSE MyCppSources "./*.cpp" "./*.cxx" "./*.c" "./*.h" "./*.hpp" "gsl/*")

# https://stackoverflow.com/a/36667361
# disable ZERO_CHECK
# set(CMAKE_SUPPRESS_REGENERATION true)

add_executable(Client ${MyCppSources})

# add the binary tree to the search path for include files
# so that we will find BuildAppConfig.h.h
target_include_directories(Client PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${MyCppSources})

# https://cmake.org/cmake/help/latest/prop_dir/VS_STARTUP_PROJECT.html
# https://stackoverflow.com/questions/7304625/how-do-i-change-the-startup-project-of-a-visual-studio-solution-via-cmake
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Client)

if(MSVC)
 target_compile_options(Client PRIVATE "/MP")
 # Enable updated __cplusplus macro
 # https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

# GeneratedCMakeLists.txt contain vars with paths to includes, libs, dlls for 3rd party libs
include(${CMAKE_CURRENT_SOURCE_DIR}/../!!BUILD_LIB/GeneratedCMakeLists.txt)
