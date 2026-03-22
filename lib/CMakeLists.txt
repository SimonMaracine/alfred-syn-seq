cmake_minimum_required(VERSION 3.20)

include(cmake/fftw.cmake)

add_library(alfred_lib STATIC)

target_sources(
    alfred_lib
    PRIVATE
        "src/audio.cpp"
        "src/instruments.cpp"
        "src/math.cpp"
        "src/preset.cpp"
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
            "include/alfred/instruments.hpp"
            "include/alfred/math.hpp"
            "include/alfred/preset.hpp"
            "include/alfred/synthesis.hpp"
            "include/alfred/synthesizer.hpp"
)

target_link_libraries(alfred_lib PRIVATE SDL3::SDL3 fftw3)

# On Windows need to explicitly link to the lib file containing the function references for the dll
if(WIN32)
    target_link_libraries(alfred_lib PRIVATE libfftw3-3.lib)
endif()

set_target_cpp_mode(alfred_lib)
set_target_warnings(alfred_lib)
enable_target_diagnostics(alfred_lib)
enable_target_ipo(alfred_lib)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_lib PRIVATE "NDEBUG")
endif()

set_target_properties(alfred_lib PROPERTIES OUTPUT_NAME "alfred")
