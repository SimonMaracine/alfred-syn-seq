cmake_minimum_required(VERSION 3.20)

add_library(alfred_core STATIC
    "include/alfred/audio.hpp"
    "include/alfred/math.hpp"
    "include/alfred/synthesis.hpp"
    "include/alfred/synthesizer.hpp"
    "src/audio.cpp"
    "src/synthesis.cpp"
    "src/synthesizer.cpp"
)

target_include_directories(alfred_core PUBLIC "include")

target_link_libraries(alfred_core PRIVATE SDL3::SDL3)

target_compile_features(alfred_core PRIVATE cxx_std_23)
set_target_properties(alfred_core PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX)
    target_compile_options(alfred_core PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(WIN32)
    target_compile_options(alfred_core PRIVATE "/W4")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
    target_compile_options(alfred_core PRIVATE "-g")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND ALFRED_SOUND_ASAN)
    target_compile_options(alfred_core PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
    target_link_options(alfred_core PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(alfred_core PRIVATE "NDEBUG")
endif()
