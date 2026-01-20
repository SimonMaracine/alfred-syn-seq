cmake_minimum_required(VERSION 3.20)

include(cmake/imgui.cmake)

add_custom_command(
    OUTPUT "imgui.ini.hpp"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate.py"
    ARGS "SETTINGS" "${CMAKE_CURRENT_SOURCE_DIR}/res/imgui.ini"
    DEPENDS "res/imgui.ini"
)

add_executable(
    alfred
    "imgui.ini.hpp"
    "src/application.cpp"
    "src/application.hpp"
    "src/imgui.cpp"
    "src/imgui.hpp"
    "src/logging.hpp"
    "src/main.cpp"
    "src/sequencer.cpp"
    "src/sequencer.hpp"
    "src/task.cpp"
    "src/task.hpp"
    "src/ui.cpp"
    "src/ui.hpp"
    "src/video.cpp"
    "src/video.hpp"
)

target_include_directories(alfred PRIVATE "src")
target_include_directories(alfred PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

target_link_libraries(alfred PRIVATE SDL3::SDL3 imgui alfred_lib)

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

if(UNIX)
    target_compile_options(alfred PRIVATE "-Wno-parentheses")
endif()
