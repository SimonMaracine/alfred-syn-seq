cmake_minimum_required(VERSION 3.20)

project(synth_sound)

option(SYNTH_SOUND_ASAN "Whether to build with sanitizers on Linux or not" OFF)

set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Variable used to control the build type")
set(BUILD_SHARED_LIBS OFF)

add_subdirectory(synthesizer)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
