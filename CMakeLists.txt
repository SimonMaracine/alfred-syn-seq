cmake_minimum_required(VERSION 3.20)

project(alfred)

option(ALFRED_DISTRIBUTION "Whether to build for distribution" OFF)
option(ALFRED_ASAN "Whether to build with sanitizers on Linux or not" OFF)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type")
set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Build types")
set(BUILD_SHARED_LIBS OFF)

include(cmake/sanity_checks.cmake)
include(cmake/target_options.cmake)

add_subdirectory(alfred)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
