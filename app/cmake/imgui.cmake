if(NOT ALFRED_DISTRIBUTION)
    set(IMGUI_DEMO "extern/imgui/imgui_demo.cpp")
endif()

add_library(imgui STATIC)

target_sources(
    imgui
    PRIVATE
        ${IMGUI_DEMO}
        "extern/imgui/imgui_draw.cpp"
        "extern/imgui/imgui_tables.cpp"
        "extern/imgui/imgui_widgets.cpp"
        "extern/imgui/imgui.cpp"
        "extern/imgui/backends/imgui_impl_sdl3.cpp"
        "extern/imgui/backends/imgui_impl_sdlrenderer3.cpp"
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            "extern/imgui"
        FILES
            "extern/imgui/imconfig.h"
            "extern/imgui/imgui_internal.h"
            "extern/imgui/imgui.h"
            "extern/imgui/imstb_rectpack.h"
            "extern/imgui/imstb_textedit.h"
            "extern/imgui/imstb_truetype.h"
            "extern/imgui/backends/imgui_impl_sdl3.h"
            "extern/imgui/backends/imgui_impl_sdlrenderer3.h"
)

target_include_directories(imgui PRIVATE "${ALFRED_SDL_INCLUDE_DIRECTORY}")

set_target_cpp_mode(imgui)

target_compile_definitions(imgui PUBLIC "IMGUI_DEFINE_MATH_OPERATORS" "IMGUI_DISABLE_OBSOLETE_FUNCTIONS")

if(ALFRED_DISTRIBUTION)
    target_compile_definitions(imgui PRIVATE "NDEBUG")
endif()
