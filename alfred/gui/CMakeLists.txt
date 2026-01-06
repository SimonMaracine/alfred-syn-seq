cmake_minimum_required(VERSION 3.20)

include(cmake/imgui.cmake)

add_executable(alfred
    "src/application.cpp"
    "src/application.hpp"
    "src/imgui.cpp"
    "src/imgui.hpp"
    "src/main.cpp"
    "src/sequencer.cpp"
    "src/sequencer.hpp"
    "src/video.cpp"
    "src/video.hpp"
)

target_include_directories(alfred PRIVATE "src")

target_link_libraries(alfred PRIVATE SDL3::SDL3 imgui alfred_core)

target_compile_features(alfred PRIVATE cxx_std_23)
set_target_properties(alfred PROPERTIES CXX_EXTENSIONS OFF)

if(UNIX)
    target_compile_options(alfred PRIVATE "-Wall" "-Wextra" "-Wpedantic" "-Wconversion")
elseif(WIN32)
    target_compile_options(alfred PRIVATE "/W4")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
    target_compile_options(alfred PRIVATE "-g")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX AND ALFRED_SOUND_ASAN)
    target_compile_options(alfred PRIVATE "-fsanitize=address" "-fsanitize=undefined" "-fno-omit-frame-pointer")
    target_link_options(alfred PRIVATE "-fsanitize=address" "-fsanitize=undefined")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_definitions(alfred PRIVATE "NDEBUG")
endif()
