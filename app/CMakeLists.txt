cmake_minimum_required(VERSION 3.20)

include(cmake/cereal.cmake)
include(cmake/imgui.cmake)

function(add_resource variable_name file_name file_type)
    add_custom_command(
        OUTPUT "include/${file_name}.hpp"
        COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/embed.py"
        ARGS "${variable_name}" "${CMAKE_CURRENT_SOURCE_DIR}/res/${file_name}" "${file_type}"
        DEPENDS "res/${file_name}"
        VERBATIM
    )
endfunction()

add_resource("ICON64" "icon64.png" "binary")
add_resource("ICON128" "icon128.png" "binary")
add_resource("PLAY" "play.png" "binary")
add_resource("PAUSE" "pause.png" "binary")
add_resource("REWIND" "rewind.png" "binary")

add_custom_command(
    OUTPUT "include/version.hpp"
    COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/version.py"
    ARGS "${CMAKE_PROJECT_VERSION}"
    VERBATIM
)

if(WIN32)
    if(ALFRED_DISTRIBUTION)
        # This also disables printing
        set(ALFRED_APP_NO_CONSOLE WIN32)
    endif()

    set(ALFRED_APP_ICON "res/alfred.rc")
endif()

add_executable(
    alfred_app
    ${ALFRED_APP_NO_CONSOLE}
    ${ALFRED_APP_ICON}
    "include/icon64.png.hpp"
    "include/icon128.png.hpp"
    "include/play.png.hpp"
    "include/pause.png.hpp"
    "include/rewind.png.hpp"
    "include/version.hpp"
    "src/application.cpp"
    "src/application.hpp"
    "src/composition.hpp"
    "src/composition.cpp"
    "src/data.cpp"
    "src/data.hpp"
    "src/encoder.cpp"
    "src/encoder.hpp"
    "src/flat_set.hpp"
    "src/imgui.cpp"
    "src/imgui.hpp"
    "src/logging.cpp"
    "src/logging.hpp"
    "src/main.cpp"
    "src/preset.cpp"
    "src/preset.hpp"
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

target_include_directories(alfred_app PRIVATE "src" "${CMAKE_CURRENT_BINARY_DIR}/include")

target_link_libraries(alfred_app PRIVATE SDL3::SDL3 cereal imgui alfred_lib)

set_target_cpp_mode(alfred_app)
set_target_warnings(alfred_app)
enable_target_diagnostics(alfred_app)

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(alfred_app PRIVATE "NDEBUG" "ALFRED_DISTRIBUTION")
endif()

if(UNIX)
    target_compile_definitions(alfred_app PRIVATE "ALFRED_LINUX")
elseif(WIN32)
    target_compile_definitions(alfred_app PRIVATE "ALFRED_WINDOWS")
endif()

if(UNIX)
    target_compile_options(alfred_app PRIVATE "-Wno-parentheses")
endif()

set_target_properties(alfred_app PROPERTIES OUTPUT_NAME "alfred")

set(ALFRED_DESKTOP_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/alfred.desktop" PARENT_SCOPE)
set(ALFRED_ICON_FILE "${CMAKE_CURRENT_SOURCE_DIR}/res/alfred.svg" PARENT_SCOPE)

# Takes care of doing the right stuff on any platform
copy_fftw3_dll(alfred_app)
