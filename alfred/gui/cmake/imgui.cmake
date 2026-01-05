add_library(imgui STATIC
    "extern/imgui/imgui_demo.cpp"
    "extern/imgui/imgui_draw.cpp"
    "extern/imgui/imgui_tables.cpp"
    "extern/imgui/imgui_widgets.cpp"
    "extern/imgui/imgui.cpp"
    "extern/imgui/imconfig.h"
    "extern/imgui/imgui_internal.h"
    "extern/imgui/imgui.h"
    "extern/imgui/imstb_rectpack.h"
    "extern/imgui/imstb_textedit.h"
    "extern/imgui/imstb_truetype.h"

    "extern/imgui/backends/imgui_impl_sdl3.cpp"
    "extern/imgui/backends/imgui_impl_sdl3.h"
    "extern/imgui/backends/imgui_impl_sdlrenderer3.cpp"
    "extern/imgui/backends/imgui_impl_sdlrenderer3.h"
)

target_include_directories(imgui PUBLIC "extern/imgui")
target_include_directories(imgui PRIVATE "../extern/SDL/include")

target_compile_definitions(imgui PUBLIC "IMGUI_DEFINE_MATH_OPERATORS")
