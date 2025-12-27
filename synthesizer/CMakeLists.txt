cmake_minimum_required(VERSION 3.20)

include(cmake/sdl.cmake)

add_executable(synthesizer
    "src/application.cpp"
    "src/application.hpp"
    "src/audio.cpp"
    "src/audio.hpp"
    "src/main.cpp"
)

target_include_directories(synthesizer PRIVATE "src")

target_link_libraries(synthesizer PRIVATE SDL3::SDL3)

target_compile_features(synthesizer PRIVATE cxx_std_20)
set_target_properties(synthesizer PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX)
    target_compile_options(synthesizer PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(WIN32)
    target_compile_options(synthesizer PRIVATE "/W4")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
    target_compile_options(synthesizer PRIVATE "-g")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND SYNTH_SOUND_ASAN)
    target_compile_options(synthesizer PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
    target_link_options(synthesizer PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(synthesizer PUBLIC "NDEBUG")
endif()
