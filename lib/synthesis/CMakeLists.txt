cmake_minimum_required(VERSION 3.20)

include(cmake/fftw.cmake)
include(cmake/hashed_string.cmake)
include(cmake/static_allocator.cmake)

add_library(alfred_lib_synthesis STATIC)

target_sources(
    alfred_lib_synthesis
    PRIVATE
        "src/math.cpp"
        "src/preset.cpp"
        "src/synthesis.cpp"
        "src/synthesizer.cpp"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "include"
        FILES
            "include/alfred/definitions.hpp"
            "include/alfred/math.hpp"
            "include/alfred/preset.hpp"
            "include/alfred/synthesis.hpp"
            "include/alfred/synthesizer.hpp"
)

target_link_libraries(alfred_lib_synthesis PRIVATE fftw3)
target_link_libraries(alfred_lib_synthesis PUBLIC alfred_static_allocator alfred_hashed_string)

# On Windows need to explicitly link to the lib file containing the function references for the DLL
if(WIN32)
    target_link_libraries(alfred_lib_synthesis PRIVATE libfftw3-3.lib)
endif()

alfred_set_target_cpp_mode(alfred_lib_synthesis)
alfred_set_target_warnings(alfred_lib_synthesis)
alfred_set_target_platform_macros(alfred_lib_synthesis)
alfred_enable_target_diagnostics(alfred_lib_synthesis)
alfred_enable_target_ipo(alfred_lib_synthesis)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_lib_synthesis PRIVATE "NDEBUG")
endif()

set_target_properties(alfred_lib_synthesis PROPERTIES OUTPUT_NAME "alfred_synthesis")
