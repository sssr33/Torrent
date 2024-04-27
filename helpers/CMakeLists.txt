cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(Helpers VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Limited configurations" FORCE)

file(GLOB_RECURSE HelpersCppSources "./*.cpp" "./*.cxx" "./*.c" "./*.h" "./*.hpp" "gsl/*")

add_library(Helpers STATIC ${HelpersCppSources})

# add the binary tree to the search path for include files
# so that we will find BuildAppConfig.h.h
target_include_directories(Helpers PUBLIC "${PROJECT_BINARY_DIR}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")

# Create the source groups for source tree with root at CMAKE_CURRENT_SOURCE_DIR.
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${HelpersCppSources})

if(MSVC)
 target_compile_options(Helpers PRIVATE "/MP")
 # Enable updated __cplusplus macro
 # https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus?view=msvc-170
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()