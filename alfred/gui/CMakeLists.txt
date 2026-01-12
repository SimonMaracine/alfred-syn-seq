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
    "src/ui.hpp"
    "src/video.cpp"
    "src/video.hpp"
)

target_include_directories(alfred PRIVATE "src")

target_link_libraries(alfred PRIVATE SDL3::SDL3 imgui alfred_core)

set_target_cpp_mode(alfred)
enable_target_warnings(alfred)
enable_target_diagnostics(alfred)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred PRIVATE "NDEBUG" "ALFRED_DISTRIBUTION")
endif()

if(UNIX)
    target_compile_definitions(alfred PRIVATE "ALFRED_LINUX")
elseif(WIN32)
    target_compile_definitions(alfred PRIVATE "ALFRED_WINDOWS")
endif()
