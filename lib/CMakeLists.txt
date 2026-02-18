cmake_minimum_required(VERSION 3.20)

add_library(alfred_lib STATIC)

target_sources(
    alfred_lib
    PRIVATE
        "src/audio.cpp"
        "src/instrument.cpp"
        "src/math.cpp"
        "src/synthesis.cpp"
        "src/synthesizer.cpp"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "include"
        FILES
            "include/alfred/allocator.hpp"
            "include/alfred/audio.hpp"
            "include/alfred/hash.hpp"
            "include/alfred/instrument.hpp"
            "include/alfred/math.hpp"
            "include/alfred/synthesis.hpp"
            "include/alfred/synthesizer.hpp"
)

target_link_libraries(alfred_lib PRIVATE SDL3::SDL3 fftw3)

set_target_cpp_mode(alfred_lib)
enable_target_warnings(alfred_lib)
enable_target_diagnostics(alfred_lib)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_lib PRIVATE "NDEBUG")
endif()

set_target_properties(alfred_lib PROPERTIES OUTPUT_NAME "alfred")
