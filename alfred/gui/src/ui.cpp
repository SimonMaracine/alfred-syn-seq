#include "ui.hpp"

#include <stdexcept>
#include <cmath>
#include <cstring>

#include <imgui.h>

namespace ui {
    static void set_font(int scale) {
        const float font_size {std::floor(FONT_SIZE * float(scale))};

        ImGuiIO& io {ImGui::GetIO()};

        io.Fonts->Clear();

        ImFontConfig config;
        config.SizePixels = font_size;

        const auto font {io.Fonts->AddFontDefault(&config)};

        if (!font) {
            throw std::runtime_error("Could not add font");
        }

        io.FontDefault = font;
        io.Fonts->Build();
    }

    static void reset_style() {
        const ImGuiStyle current_style {ImGui::GetStyle()};

        ImGui::GetStyle() = ImGuiStyle();
        std::memcpy(ImGui::GetStyle().Colors, current_style.Colors, sizeof(current_style.Colors));
    }

    float rem(float size) {
        return ImGui::GetFontSize() * size;
    }

    void set_scale(int scale) {
        set_font(scale);
        reset_style();

        ImGui::GetStyle().ScaleAllSizes(float(scale));
    }
}
