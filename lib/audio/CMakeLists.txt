cmake_minimum_required(VERSION 3.20)

add_library(alfred_lib_audio STATIC)

target_sources(
    alfred_lib_audio
    PRIVATE
        "src/audio.cpp"
        "src/real_synthesizer.cpp"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "include"
        FILES
            "include/alfred/audio.hpp"
            "include/alfred/real_synthesizer.hpp"
)

target_link_libraries(alfred_lib_audio PRIVATE SDL3::SDL3 alfred_lib_synthesis)

set_target_cpp_mode(alfred_lib_audio)
set_target_warnings(alfred_lib_audio)
enable_target_diagnostics(alfred_lib_audio)
enable_target_ipo(alfred_lib_audio)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_lib_audio PRIVATE "NDEBUG")
endif()

set_target_properties(alfred_lib_audio PROPERTIES OUTPUT_NAME "alfred_audio")
