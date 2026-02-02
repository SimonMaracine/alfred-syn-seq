cmake_minimum_required(VERSION 3.20)

project(alfred)

include(cmake/options.cmake)

set(CACHE{CMAKE_BUILD_TYPE} TYPE STRING VALUE "Debug")
set(CACHE{CMAKE_CONFIGURATION_TYPES} TYPE STRINGS FORCE VALUE "Debug;Release")
set(BUILD_SHARED_LIBS OFF)

include(cmake/sanity_checks.cmake)
include(cmake/target_options.cmake)
include(cmake/sdl.cmake)

add_subdirectory(lib)
add_subdirectory(app)

include(cmake/install.cmake)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

message(STATUS "ALFRED_DISTRIBUTION: ${ALFRED_DISTRIBUTION}")
message(STATUS "ALFRED_ASAN: ${ALFRED_ASAN}")
message(STATUS "ALFRED_ASAN_THREAD: ${ALFRED_ASAN_THREAD}")
message(STATUS)
