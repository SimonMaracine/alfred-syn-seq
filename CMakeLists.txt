cmake_minimum_required(VERSION 3.20)

project(alfred)

option(ALFRED_DISTRIBUTION "Whether to build for distribution" OFF)
option(ALFRED_ASAN "Whether to build with sanitizers on Linux or not" OFF)

set(CACHE{CMAKE_BUILD_TYPE} TYPE STRING VALUE "Debug")
set(CACHE{CMAKE_CONFIGURATION_TYPES} TYPE STRINGS VALUE "Debug;Release")
set(BUILD_SHARED_LIBS OFF)

include(cmake/sanity_checks.cmake)
include(cmake/target_options.cmake)

include(cmake/sdl.cmake)
add_subdirectory(lib)
add_subdirectory(app)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "ALFRED_DISTRIBUTION: ${ALFRED_DISTRIBUTION}")
message(STATUS "ALFRED_ASAN: ${ALFRED_ASAN}")
