cmake_minimum_required(VERSION 3.20)

add_library(alfred_lib_instruments STATIC)

target_sources(
    alfred_lib_instruments
    PRIVATE
        "src/instruments.cpp"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "include"
        FILES
            "include/alfred/instruments.hpp"
)

target_link_libraries(alfred_lib_instruments PRIVATE alfred_lib_synthesis)

set_target_cpp_mode(alfred_lib_instruments)
set_target_warnings(alfred_lib_instruments)
enable_target_diagnostics(alfred_lib_instruments)
enable_target_ipo(alfred_lib_instruments)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_lib_instruments PRIVATE "NDEBUG")
endif()

set_target_properties(alfred_lib_instruments PROPERTIES OUTPUT_NAME "alfred_instruments")
