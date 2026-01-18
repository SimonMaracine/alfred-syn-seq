cmake_minimum_required(VERSION 3.20)

add_library(
    alfred_core STATIC
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

set_target_cpp_mode(alfred_core)
enable_target_warnings(alfred_core)
enable_target_diagnostics(alfred)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_core PRIVATE "NDEBUG")
endif()
