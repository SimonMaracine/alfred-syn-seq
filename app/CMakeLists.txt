cmake_minimum_required(VERSION 3.20)

include(cmake/imgui.cmake)

function(add_resource variable_name file_name file_type)
    add_custom_command(
        OUTPUT "include/${file_name}.hpp"
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/generate.py"
        ARGS "${variable_name}" "${CMAKE_CURRENT_SOURCE_DIR}/res/${file_name}" "${file_type}"
        DEPENDS "res/${file_name}"
        VERBATIM
    )
endfunction()

add_resource("SETTINGS" "imgui.ini" "text")
add_resource("ICON64" "icon64.png" "binary")
add_resource("ICON128" "icon128.png" "binary")
add_resource("PLAY" "play.png" "binary")
add_resource("PAUSE" "pause.png" "binary")
add_resource("REWIND" "rewind.png" "binary")

add_executable(
    alfred
    "include/imgui.ini.hpp"
    "include/icon64.png.hpp"
    "include/icon128.png.hpp"
    "include/play.png.hpp"
    "include/pause.png.hpp"
    "include/rewind.png.hpp"
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
    "src/image.cpp"
    "src/image.hpp"
    "src/ui.cpp"
    "src/ui.hpp"
    "src/utility.cpp"
    "src/utility.hpp"
    "src/video.cpp"
    "src/video.hpp"
)

target_include_directories(alfred PRIVATE "src" "${CMAKE_CURRENT_BINARY_DIR}/include")

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
